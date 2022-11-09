# TinyALSA Utils

- Capture
- Playback
- Capture and Playback in simultaneously

## Usage

1. build

    ```bash
    $ make
    ```

2. capture

    ```bash
    #./tinycap --help
    $ ./tinycap
    ```

3. playback

    ```bash
    #./tinyplay --help
    $ ./tinyplay
    ```
4. capture and playback in simultaneously

    ```bash
    #./tinycaplay --help
    $ ./tinycaplay
    ```

## Notice

If pulseaudio/ALSA don't work, you can restart it by command as follow:

```bash
$ pulseaudio -k && sudo alsa force-reload
```
