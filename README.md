# 😴 Sleep

Writing POSIX sleep() utility without any dependency- no stdio, no unistd, not even libc. Just some inline assembly magic.

> [!> [!CAUTION]
> Only supports x86_74 architecture.

## Run Yourself

Clone the repository and run the build script. The script excludes libc from compilation.

```bash
git clone https://github.com/lokop5116/safe-sleep
cd safe-sleep
./compile.sh
```

## Usage

Does not support any additional flags. Run with a simple integer value.
