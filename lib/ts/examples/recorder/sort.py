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


def write_row(path: Path, row: str):
    ensure_dir(path.parent)
    write_header = not path.exists()
    with path.open('a', encoding='utf-8', newline='') as handle:
        if write_header:
            handle.write(HEADER + '\n')
        handle.write(row + '\n')


def process_market_file(source_path: Path):
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
            write_row(output_path, row)


def main():
    if not BASE_DIR.exists() or not BASE_DIR.is_dir():
        return

    source_paths: list[Path] = []
    for source_path in sorted(BASE_DIR.glob('*/market.csv')):
        if source_path.is_file():
            source_paths.append(source_path)
            process_market_file(source_path)
    for path in source_paths:
        try:
            path.unlink()
        except Exception:
            pass


if __name__ == '__main__':
    main()
