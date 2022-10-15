# How to Contribute

## Getting Started

- Ensure you have Visual Studio 2022 installed.
- Ensure the path of the project only contains ASCII characters.
- Ensure you have Python 3.10 or later installed.
- Ensure you have Poetry 1.2 or later installed. See
  [Poetry Installation](https://python-poetry.org/docs/#installation) for
  details.

After cloning the repository, execute the install script to install the required
dependencies with vcpkg.
[Vcpkg may require elevation](https://vcpkg.io/en/getting-started.html).

```bat
.\install.bat
```

After the dependencies are installed, open the solution file in Visual Studio.
The project should be ready to build.

## Commitizen

This project uses [Commitizen](https://commitizen-tools.github.io/commitizen/)
to lint commit messages. See
[conventional commits](https://www.conventionalcommits.org/) for more
information.
