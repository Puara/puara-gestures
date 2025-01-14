# Touch Project

This project is part of the Puara Framework and showcases the integration of `puara-gestures` with a touchscreen in a [Wokwi](https://wokwi.com/) simulation to detect and display touch interactions.

## Running the Project Using Wokwi in Visual Studio Code

1. Install [Visual Studio Code](https://code.visualstudio.com/), as well as the [PlatformIO](https://platformio.org/install/ide?install=vscode) and [Wokwi](https://docs.wokwi.com/vscode/getting-started) VS Code extensions. As part of the process you'll need to create a free [Wokwi](https://wokwi.com/) online account.

2. Clone this repository and open the `exampleProjects/touch` directory in Visual Studio Code.

    ```sh
    git clone https://github.com/Puara/puara-gestures.git
    code puara-gestures/exampleProjects/touch
    ```

    Once you open the `touch` directory in VS Code, PlatformIO should take a moment to install all the dependencies listed in `exampleProjects/touch/platformio.ini`.

3. Open the `diagram.json` file.

4. If you've never signed in your wokwi account from VS Code, or it's been more than a month since you got your last license, you might get this dialog in the bottom right of the IDE. If so, click on "Request a New License" and follow the instructions to get another free Wokwi license.

<p align="center">
  <img width="450" src="https://github.com/user-attachments/assets/3502135c-9326-40aa-9887-8d796e464ff9">
</p>

5. Click on the green arrow "Start Simulation" button in the top left to build and run the project in Wokwi:

<p align="center">
  <img width="600" src="https://github.com/user-attachments/assets/f073b596-53a7-4404-a2aa-0ac49ad176b6">
</p>

6. Once the simulation is started, click on the touch screen to start the touch example. Now if you "brush" and "rub" the touch screen up and down with your mouse cursor you should see `brush` and `rub` values printed in the VS Code Wokwi Terminal:

<p align="center">
  <img width="600" src="https://github.com/user-attachments/assets/bafca748-f5d7-46ba-8f48-a05db5cf996e">
</p>

7. The firmware code to run this simulation is located in `exampleProjects/touch/src`. Feel free to explore and modify the code to understand how it all works. Have fun!

## License

This project is licensed under the MIT License.
