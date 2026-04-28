#!/usr/bin/env python3
import re
from pathlib import Path

BASE_DIR = Path('/var/lib/webctp/record')

HEADER = 'TradingDay,InstrumentID,UpdateTime,UpdateMillisec,LastPrice,Volume,BidPrice1,BidVolume1,AskPrice1,AskVolume1,AveragePrice,Turnover,OpenInterest,UpperLimitPrice,LowerLimitPrice'
FUTURES_RE = re.compile(r'^[A-Za-z]+\d+$')
PREFIX_RE = re.compile(r'^([A-Za-z]+\d+)')


def categorize(instrument_id: str):
    instrument_id = instrument_id.strip()
    if FUTURES_RE.fullmatch(instrument_id):
        return 'futures', instrument_id
    match = PREFIX_RE.match(instrument_id)
    return 'options', match.group(1) if match else instrument_id


def ensure_dir(path: Path):
    path.mkdir(parents=True, exist_ok=True)


def open_output_file(path: Path, file_handles: dict):
    if path in file_handles:
        return file_handles[path]

    ensure_dir(path.parent)
    write_header = not path.exists()
    handle = path.open('a', encoding='utf-8', newline='')
    if write_header:
        handle.write(HEADER + '\n')
    file_handles[path] = handle
    return handle


def process_market_file(source_path: Path, file_handles: dict):
    with source_path.open('r', encoding='utf-8', newline='') as source:
        header = source.readline()
        if not header:
            return

        for line in source:
            if not line.strip():
                continue

            row = line.rstrip('\r\n')
            columns = row.split(',')
            if len(columns) < 2:
                continue

            instrument_id = columns[1].strip()
            if not instrument_id:
                continue

            category, filename_key = categorize(instrument_id)
            output_path = source_path.parent / category / f'{filename_key}.csv'
            open_output_file(output_path, file_handles).write(row + '\n')


def close_file_handles(file_handles: dict):
    for handle in file_handles.values():
        try:
            handle.close()
        except Exception:
            pass


def main():
    if not BASE_DIR.exists() or not BASE_DIR.is_dir():
        return

    file_handles = {}
    source_paths: list[Path] = []
    try:
        for source_path in sorted(BASE_DIR.glob('*/market.csv')):
            if source_path.is_file():
                source_paths.append(source_path)
                process_market_file(source_path, file_handles)
    finally:
        close_file_handles(file_handles)
        for path in source_paths:
            try:
                path.unlink()
            except Exception:
                pass


if __name__ == '__main__':
    main()
