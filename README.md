Finding signals:

    rtl_fm -M wbfm -f 433.92M -g 0 > output.raw
    ./gate.rb

Analyze the signals and fill the `gate-arduino/config.ino`. Add `,0` to the end of each signal.
