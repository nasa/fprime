# Installing Autocomplete

## Autocomplete: Tab Completion for Command-Line Tools

While you can start using the GDS command line tools immediately after installing F´, they'll be missing **tab completion**, a handy feature of modern shells that will automatically try to complete the command/option you're typing when you press - surprise - the `[TAB]` key. If more than one option is available, you can hit `[TAB]` twice to display all your choices.

Currently, you can use tab completion in the GDS CLI tool to complete sub-command names, options, file paths, and even the available commands on the F´ instance you're running. If there's anything you think we're missing, let us know through
[a new GitHub issue](https://github.com/nasa/fprime/issues/new).

## How to Activate Tab Completion

If you've already installed F´ correctly and have F´s virtual environment activated, you can activate tab completion by doing the following:

### For the GDS CLI

>   **NOTE**: Each of the following commands will *only* activate tab completion for the current terminal session; if you open up a new window, you'll have to run these commands again. We're currently searching for the correct way to fix this,
>
>   For now, you *can* add some commands to the `activate` script you use to start your virtual environment (enabling tab completion whenever the virtual env starts), but **this is not good practice** as that file was never intended to be written to by the user. If you decide the convenience is worth it, here's what to run for each shell:
>
>   **Bash/zsh**: `register-python-argcomplete fprime-cli >> $VIRTUAL_ENV/bin/activate`
>
>   **fish**: `register-python-argcomplete --shell fish fprime-cli >> $VIRTUAL_ENV/bin/activate.fish`
>
>   **tcsh**: `register-python-argcomplete --shell tcsh fprime-cli >> $VIRTUAL_ENV/bin/activate.csh`
>
>   Re-activate your virtual environment for this to take effect.

#### Bash

1.  Run `eval "$(register-python-argcomplete fprime-cli)"`

#### zsh

1.  Enable `bashcompinit` by running the following:

    ```bash
    autoload -U bashcompinit
    bashcompinit
    ```

2.  Run `eval "$(register-python-argcomplete fprime-cli)"`

#### fish

1.  Run `register-python-argcomplete --shell fish fprime-cli | source`

#### tcsh

1.  Run ``eval `register-python-argcomplete --shell tcsh fprime-cli` ``

## Technical Aside

F´ has its CLI tools written in Python, and we've implemented tab completion through the [argcomplete](https://github.com/kislyuk/argcomplete) library. If you have any troubleshooting concerns, or want to extend these tools, the argcomplete documentation may be helpful.
