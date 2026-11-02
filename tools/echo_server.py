#!/usr/bin/env python3
"""Length-prefixed echo (uint16 BE + payload). Prefer in-app 'Start local echo'."""

import socket
import struct
import traceback

HOST = "127.0.0.1"
PORT = 19000


def read_exact(conn: socket.socket, n: int) -> bytes:
    buf = b""
    while len(buf) < n:
        try:
            chunk = conn.recv(n - len(buf))
        except socket.timeout:
            continue
        if not chunk:
            return b""
        buf += chunk
    return buf


def main() -> None:
    with socket.socket(socket.AF_INET, socket.SOCK_STREAM) as srv:
        srv.setsockopt(socket.SOL_SOCKET, socket.SO_REUSEADDR, 1)
        srv.bind((HOST, PORT))
        srv.listen(5)
        print(f"echo server on {HOST}:{PORT}  (Ctrl+C to stop)", flush=True)
        while True:
            srv.settimeout(0.5)
            try:
                conn, addr = srv.accept()
            except socket.timeout:
                continue
            conn.settimeout(0.5)
            print("client", addr, flush=True)
            try:
                with conn:
                    while True:
                        hdr = read_exact(conn, 2)
                        if not hdr:
                            print("eof header", flush=True)
                            break
                        (length,) = struct.unpack("!H", hdr)
                        print(f"hdr hex={hdr.hex()} len={length}", flush=True)
                        payload = read_exact(conn, length)
                        if len(payload) < length:
                            print("eof payload", flush=True)
                            break
                        print("rx", payload, flush=True)
                        conn.sendall(hdr + payload)
            except Exception:
                traceback.print_exc()
            print("client gone", flush=True)


if __name__ == "__main__":
    main()
