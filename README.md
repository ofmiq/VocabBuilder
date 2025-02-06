# 📖 VocabBuilder

VocabBuilder is a vocabulary learning application that helps users study new words by automatically fetching their definitions and exporting them into a CSV file. The application is designed to streamline the process of learning new words from a provided text file.

## ✨ Features

- Automatically retrieves word definitions via an API.

- Processes text files containing words for learning.

- Exports results into a CSV file for flashcard applications.

- Simple and user-friendly interface built with Qt.

## 🚀 Installation

### Using Prebuilt Release

1. Download the latest release from the Releases page.

2. Extract the archive.

  3. Run VocabBuilder.exe.

## 🔧 Build Steps

Follow these steps to build and run the project successfully.

### ✅ 1. Install Dependencies

Make sure you have the following installed:

- Qt 6.7.2 (or compatible version) with MinGW 64-bit

- CMake 3.16+

- MinGW (GCC 13.2.0 or compatible)

- `windeployqt` (included with Qt)

### ✅ 2. Configure and Build the Project

1. Clone the repository:
``` sh
git clone https://github.com/ofmiq/VocabBuilder.git
cd VocabBuilder
```

2. Create a build/ directory and navigate into it:
``` sh
mkdir build
cd build
```

3. Run CMake to generate the Makefiles:
``` sh
cmake .. -G "MinGW Makefiles" -DCMAKE_PREFIX_PATH="YOUR_PATH_TO_QT/lib/cmake" -DCMAKE_C_COMPILER="YOUR_PATH_TO_MINGW/bin/gcc.exe" -DCMAKE_CXX_COMPILER="YOUR_PATH_TO_MINGW/bin/g++.exe"
```

4. Compile the project:
``` sh
mingw32-make
```

### ✅ 3. Deploy the Executable

Run windeployqt to copy required Qt libraries:
``` sh
"YOUR_PATH_TO_windeployqt.exe" --release VocabBuilder.exe
```

Now you can run VocabBuilder.exe from the deployment folder.

## 📝 Usage

1. Prepare a text file containing words you want to learn.

2. Load the file into the application.

3. The application fetches definitions and updates the file.

4. Download the processed file in CSV or TXT format.

## 🛠 Contributing

Feel free to submit issues, feature requests, or pull requests to improve the application.

## 📄 License

This project is licensed under the MIT License. See the [LICENSE](https://github.com/ofmiq/VocabBuilder/blob/main/LICENSE) file for details

