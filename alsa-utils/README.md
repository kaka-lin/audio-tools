# ALSA Utils

## Usage

1. build

    ```bash
    $ make
    ```

2. run

    ```bash
    $ ./alsa-example
    ```

## Notice

If pulseaudio/ALSA don't work, you can restart it by command as follow:

```bash
$ pulseaudio -k && sudo alsa force-reload
```
