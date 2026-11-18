"""Подготовка XML из реальной выгрузки Москвы; в работе приложения не участвует."""

import argparse
import collections
import csv
import hashlib
import io
import json
from pathlib import Path
import re
import xml.etree.ElementTree as ET
import zipfile


def read_records(path):
    content = path.read_bytes()
    if zipfile.is_zipfile(io.BytesIO(content)):
        with zipfile.ZipFile(io.BytesIO(content)) as archive:
            names = [name for name in archive.namelist() if name.lower().endswith('.json')]
            if len(names) != 1:
                raise ValueError('Ожидается архив с одним JSON-файлом данных')
            content = archive.read(names[0])
    try:
        text = content.decode('utf-8-sig')
    except UnicodeDecodeError:
        text = content.decode('cp1251')
    if path.suffix.lower() == '.csv':
        columns = ['ID', 'Name', 'global_id', 'IsNetObject', 'OperatingCompany',
                   'TypeObject', 'AdmArea', 'District', 'Address', 'PublicPhone',
                   'SeatsCount', 'SocialPrivileges', 'Longitude_WGS84',
                   'Latitude_WGS84', 'geoData']
        records = list(csv.DictReader(io.StringIO(text), fieldnames=columns))
        for record in records:
            try:
                record['global_id'] = int(record['global_id'])
            except (ValueError, TypeError):
                record['global_id'] = None
    elif text.lstrip().startswith('['):
        records = json.loads(text)
    else:
        records = [json.loads(line) for line in text.splitlines() if line.strip()]
    # Первая строка некоторых выгрузок содержит русские названия столбцов.
    return [row.get('Cells', row) for row in records
            if isinstance(row, dict) and isinstance(row.get('global_id'), int)]


def scalar(value):
    return '' if value is None else str(value)


def phones(value):
    if isinstance(value, list):
        return [number for part in value for number in phones(part)]
    if isinstance(value, dict):
        return phones(value.get('PublicPhone'))
    return [str(value)] if value else []


def write_xml(records, destination, category):
    root = ET.Element('catering', category=category)
    for record in records:
        organization = ET.SubElement(root, 'organization')
        for key in ['global_id', 'Name', 'TypeObject', 'AdmArea', 'District', 'Address',
                    'SeatsCount', 'Latitude_WGS84', 'Longitude_WGS84']:
            value = record.get(key)
            # GeoJSON хранит координаты в порядке долгота, широта.
            if (value is None or value == '') and key.endswith('_WGS84'):
                coordinates = (record.get('geoData') or {}).get('coordinates') or []
                if len(coordinates) == 2:
                    value = coordinates[1 if key.startswith('Latitude') else 0]
            ET.SubElement(organization, key).text = scalar(value)
        group = ET.SubElement(organization, 'PublicPhone')
        for phone in phones(record.get('PublicPhone')):
            ET.SubElement(group, 'phone').text = phone
    ET.indent(root, space='  ')
    ET.ElementTree(root).write(destination, encoding='utf-8', xml_declaration=True)


def main():
    parser = argparse.ArgumentParser(description=__doc__)
    parser.add_argument('source', type=Path, help='CSV, JSON, JSONL или ZIP с JSON')
    parser.add_argument('--output', type=Path, default=Path(__file__).resolve().parents[1] / 'data')
    parser.add_argument('--source-url', required=True)
    parser.add_argument('--dataset-date', required=True)
    args = parser.parse_args()
    records = read_records(args.source)
    if not records:
        raise ValueError('В исходном файле нет записей с числовым global_id')
    types = collections.Counter(str(record.get('TypeObject', '')).strip().lower()
                                for record in records)
    coffee_type = 'кофейня' in types
    groups = {
        'cafes.xml': ('Кафе', [r for r in records if r.get('TypeObject', '').strip().lower() == 'кафе']),
        'coffeehouses.xml': ('Кофейни', [r for r in records if
            r.get('TypeObject', '').strip().lower() == 'кофейня' or (not coffee_type and
            re.search(r'кофе|coffee|cofix|starbucks|шоколадниц|costa', r.get('Name', ''), re.I))]),
        'bars.xml': ('Бары', [r for r in records if r.get('TypeObject', '').strip().lower() == 'бар']),
        'fast_food.xml': ('Быстрое обслуживание', [r for r in records if
            r.get('TypeObject', '').strip().lower() == 'предприятие быстрого обслуживания']),
    }
    if any(not group[1] for group in groups.values()):
        raise ValueError('Одна из категорий пуста; доступные типы: ' + repr(types))
    args.output.mkdir(parents=True, exist_ok=True)
    outputs = {}
    for filename, (category, rows) in groups.items():
        write_xml(rows, args.output / filename, category)
        outputs[filename] = len(rows)
    metadata = {
        'dataset': 'Общественное питание в Москве, набор 1903',
        'portal': 'https://data.mos.ru/opendata/1903',
        'source_url': args.source_url,
        'dataset_date': args.dataset_date,
        'source_file': args.source.name,
        'source_sha256': hashlib.sha256(args.source.read_bytes()).hexdigest(),
        'source_records': len(records),
        'source_types': dict(sorted(types.items())),
        'xml_files': outputs,
        'coffee_selection': 'TypeObject = кофейня' if coffee_type else
            'Название соответствует регулярному выражению кофе|coffee|cofix|starbucks|шоколадниц|costa; '
            'это эвристическая выборка, может пересекаться с другими категориями',
        'transformation': 'Исходная выгрузка -> XML UTF-8. Идентификаторы и исходные значения сохранены. '
            'Координаты из geoData используются только при отсутствии отдельных полей WGS84.',
    }
    (args.output / 'provenance.json').write_text(
        json.dumps(metadata, ensure_ascii=False, indent=2) + '\n', encoding='utf-8')
    print(json.dumps(outputs, ensure_ascii=False))


if __name__ == '__main__':
    main()
