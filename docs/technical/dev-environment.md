# Development environment setup

!!! warning
    If you want to just use Euphonium on your hardware, you probably want to use our prebuilt version. This guide is for people who want to get deep into the code, and develop the software's internals.

Euphonium uses quite a wide variety of tools internally. We make use of the [Nix package manager](https://nixos.org/), in order to make the setup process consistent on all platforms.

## Checkout code

When checking out the repository, use `--recursive` to retrieve all submodules. Alternately, after checkout use `git submodule update --init --recursive` to perform the same task.

```
git clone --recursive https://github.com/muvox-io/euphonium
```

