//****************************************************************************//
// Puara Gestures - Sensor fusion and  gestural descriptors algorithms        //
// Metalab - Société des Arts Technologiques (SAT)                            //
// Input Devices and Music Interaction Laboratory (IDMIL), McGill University  //
// Edu Meneses (2022-2024) - https://github.com/edumeneses                    //
//****************************************************************************//

class SecondClass {
        private:
            const FirstClass& firstInstance;
            const ThirdClass& thirdInstance;

        public:
            SecondClass(const FirstClass& first, const ThirdClass& third) : firstInstance(first), thirdInstance(third) {}

            void printFirstValue() {
                std::cout << "Value in FirstClass: " << firstInstance.value << std::endl;
                std::cout << "Values in ThirdClass: " << thirdInstance.value1 << ", " << thirdInstance.value2 << std::endl;
            }
        };