//********************************************************************************//
// Puara Gestures - Calibration (.h)                                              //
// https://github.com/Puara/puara-gestures                                        //
// Société des Arts Technologiques (SAT) - https://sat.qc.ca                      //
// Input Devices and Music Interaction Laboratory (IDMIL) - https://www.idmil.org //
// Edu Meneses (2024) - https://www.edumeneses.com                                //
// Rochana Fardon (2024)                                                          //
//********************************************************************************//

#pragma once

#include <puara/structs.h>

#include <Eigen/Core>
#include <Eigen/Dense>
#include <unsupported/Eigen/MatrixFunctions>

namespace puara_gestures::utils
{

/**
 *  Calibrates the raw magnetometer values 
 */
class Calibration
{
public:
  Imu9Axis myCalIMU;
  std::vector<Coord3D> rawMagData;
  Eigen::MatrixXd softIronMatrix;
  Eigen::VectorXd hardIronBias;

  Calibration()
      : softIronMatrix(3, 3)
      , hardIronBias(3)
  {
    softIronMatrix << 1, 1, 1, 1, 1, 1, 1, 1, 1;

    hardIronBias << 0, 0, 0;
  }

  /**
   *  The gravitation field is used to calculate the soft iron matrices and should be modified according to the follownig formula:
   *       1- Get the Total Field for your location from: 
   *          https://www.ngdc.noaa.gov/geomag/calculators/magcalc.shtml#igrfwmm
   *       2- Convert the Total Field value to Gauss (1nT = 10E-5G)
   *       3- Convert Total Field to Raw value Total Field, which is the
   *          Raw Gravitation Field we are searching for
   *          Read your magnetometer datasheet and find your gain value,
   *          Which should be the same of the collected raw points
   *
   *  Reference: https://github.com/nliaudat/magnetometer_calibration/blob/main/calibrate.py
   *  
   *
   */
  int gravitationField
      = 234; // should be 1000 by default, this was calculated for the LSM9DS1 in Montreal

  void applyMagnetometerCalibration(const Imu9Axis& myRawIMU)
  {
    myCalIMU.magn.x = softIronMatrix(0, 0) * (myRawIMU.magn.x - hardIronBias(0))
                      + softIronMatrix(0, 1) * (myRawIMU.magn.y - hardIronBias(1))
                      + softIronMatrix(0, 2) * (myRawIMU.magn.z - hardIronBias(2));

    myCalIMU.magn.y = softIronMatrix(1, 0) * (myRawIMU.magn.x - hardIronBias(0))
                      + softIronMatrix(1, 1) * (myRawIMU.magn.y - hardIronBias(1))
                      + softIronMatrix(1, 2) * (myRawIMU.magn.z - hardIronBias(2));

    myCalIMU.magn.z = softIronMatrix(2, 0) * (myRawIMU.magn.x - hardIronBias(0))
                      + softIronMatrix(2, 1) * (myRawIMU.magn.y - hardIronBias(1))
                      + softIronMatrix(2, 2) * (myRawIMU.magn.z - hardIronBias(2));
  }

  /**
   * Records the raw magnetometer data and saves it in a vector.
   * The user needs to call this a minimum amount of time in order to generate at least 1500 data points
   */
  int recordRawMagData(const Coord3D& magData)
  {
    rawMagData.push_back(magData);

    return rawMagData.size();
  }

  /**
   * Fits an ellipsoid to 3D points by creating matrices from the input, solving for eigenvalues, 
   * and returning the ellipsoid's shape matrix M, center vector n, and scalar offset d
   */
  std::tuple<Eigen::MatrixXd, Eigen::VectorXd, double>
  ellipsoid_fit(const Eigen::MatrixXd& s)
  {

    // std::cout << "s " << std::endl << s << std::endl;

    Eigen::MatrixXd D(10, s.cols());

    D.row(0) = s.row(0).array().square();
    D.row(1) = s.row(1).array().square();
    D.row(2) = s.row(2).array().square();
    D.row(3) = 2.0 * s.row(1).array() * s.row(2).array();
    D.row(4) = 2.0 * s.row(0).array() * s.row(2).array();
    D.row(5) = 2.0 * s.row(0).array() * s.row(1).array();
    D.row(6) = 2.0 * s.row(0).array();
    D.row(7) = 2.0 * s.row(1).array();
    D.row(8) = 2.0 * s.row(2).array();
    D.row(9) = Eigen::MatrixXd::Ones(1, 105);

    // std::cout << std::fixed << std::setprecision(30);

    // std::cout << std::scientific << std::setprecision(8) << "D " << std::endl << D << std::endl;

    Eigen::MatrixXd S = D * D.transpose();

    Eigen::MatrixXd S_11 = S.block(0, 0, 6, 6);
    Eigen::MatrixXd S_12 = S.block(0, 6, 6, S.cols() - 6);
    Eigen::MatrixXd S_21 = S.block(6, 0, S.rows() - 6, 6);
    Eigen::MatrixXd S_22 = S.block(6, 6, S.rows() - 6, S.cols() - 6);

    // std::cout << "S " << std::endl << S << std::endl;

    Eigen::MatrixXd C(6, 6);

    C << -1, 1, 1, 0, 0, 0, 1, -1, 1, 0, 0, 0, 1, 1, -1, 0, 0, 0, 0, 0, 0, -4, 0, 0, 0,
        0, 0, 0, -4, 0, 0, 0, 0, 0, 0, -4;

    Eigen::MatrixXd C_inv = C.inverse();

    Eigen::MatrixXd E = C_inv * (S_11 - (S_12 * (S_22.inverse() * S_21)));

    Eigen::EigenSolver<Eigen::MatrixXd> solver(E);
    Eigen::VectorXd E_w = solver.eigenvalues().real();
    Eigen::MatrixXd E_v = solver.eigenvectors().real();

    int maxIndex = 0;
    E_w.maxCoeff(&maxIndex);
    Eigen::VectorXd v_1 = E_v.col(maxIndex);

    if(v_1(0) < 0)
      v_1 = -v_1;

    Eigen::VectorXd v_2 = (-S_22.inverse() * S_21) * v_1;

    Eigen::MatrixXd M(3, 3);
    Eigen::VectorXd n(3);
    double d = v_2[3];

    M << v_1(0), v_1(5), v_1(4), v_1(5), v_1(1), v_1(3), v_1(4), v_1(3), v_1(2);

    n << v_2(0), v_2(1), v_2(2);

    return std::make_tuple(M, n, d);
  }

  /**
   * Generates magnetometer calibration matrices based on saved raw dataset by fitting an ellipsoid to a set of 3D coordinates, 
   * deriving the hard-iron bias and soft-iron matrix based on  the pre-defined gravitational field.
   */
  int generateMagnetometerMatrices(std::vector<Coord3D> customRawMagData)
  {

    if(customRawMagData.empty())
    {
      return 0;
    }

    Eigen::MatrixXd s(customRawMagData.size(), 3);

    for(int i = 0; i < customRawMagData.size(); ++i)
    {
      s(i, 0) = customRawMagData[i].x;
      s(i, 1) = customRawMagData[i].y;
      s(i, 2) = customRawMagData[i].z;
    }

    Eigen::MatrixXd M;
    Eigen::VectorXd n;
    double d;

    std::tie(M, n, d) = ellipsoid_fit(s.transpose());

    Eigen::MatrixXd M_1 = M.inverse();
    hardIronBias = -(M_1 * n);

    softIronMatrix
        = (gravitationField / std::sqrt((n.transpose() * (M_1 * n) - d)) * M.sqrt());

    return 1;
  }

  void generateMagnetometerMatrices() { generateMagnetometerMatrices(rawMagData); }
};

}
