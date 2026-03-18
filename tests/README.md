To run tests, move into puara-gestures/tests/ folder and do :

```bash
mkdir -p build && cd build
cmake ../
cmake --build .
ctest -V
```

When you are done, remove the build folder by moving into the test folder level and deleting build/ :

```bash
cd ..
rm -rf build
```