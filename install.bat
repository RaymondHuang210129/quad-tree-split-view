:: Integrate vcpkg with Visual Studio

git clone https://github.com/Microsoft/vcpkg.git
call .\vcpkg\bootstrap-vcpkg.bat
.\vcpkg\vcpkg.exe integrate install
.\vcpkg\vcpkg.exe install --triplet "x64-windows" "--x-install-root=vcpkg_installed\x64-windows\\"

:: Install pre-commit hooks with Poetry

poetry install
poetry run pre-commit install
