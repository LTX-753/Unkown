import argparse
import json
import serial


def main():
    parser = argparse.ArgumentParser(description="Listen for knob display JSON events.")
    parser.add_argument("--port", default="COM3")
    parser.add_argument("--baud", type=int, default=115200)
    args = parser.parse_args()

    with serial.Serial(args.port, args.baud, timeout=1) as ser:
        while True:
            line = ser.readline().decode("utf-8", errors="replace").strip()
            if not line:
                continue
            try:
                print(json.dumps(json.loads(line), ensure_ascii=False))
            except json.JSONDecodeError:
                print(line)


if __name__ == "__main__":
    main()
