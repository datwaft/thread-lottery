# How do I contribute to the project?

Before even starting to edit code in this repository you should install the git hooks. This prevents bad code from even going into the repository.

For this you need to have [`pre-commit`](https://pre-commit.com) installed.

```sh
make install-hooks
```

After that you can start to contribute to the project.

Only remember that every commit and PR title must follow the [Conventional Commits](https://www.conventionalcommits.org/en/v1.0.0/) standard.

## F.A.Q

### How do I execute the target?

```sh
make
./build/main
```

The first command compiles the project and the second command executes the target.

### How do I execute the unit tests?

You need to have [`Criterion`](https://github.com/Snaipe/Criterion) installed.

```sh
make test
```

### How do I zip this project into a `.tgz` file?

```sh
make dist
```

After that you will see a `.tgz` file at the root level of the project.

### My IDE is showing errors that some header files are not found

<img width="682" alt="image" src="https://user-images.githubusercontent.com/37723586/222520911-1a1279bb-fa6e-4a96-a69e-8ce1b0d7d7ac.png">

To solve this you need to generate your `compile_commands.json` file. This file is particular for every maintainer so it is git ignored.

```sh
make clean
bear -- make all all_tests
```

For this you need to have [`bear`](https://github.com/rizsotto/Bear) installed. You also need to have [`Criterion`](https://github.com/Snaipe/Criterion) installed because test files require it.
