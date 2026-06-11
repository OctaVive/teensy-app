import hashlib
import logging
import xml.etree.ElementTree as ET
from dataclasses import dataclass, field
from datetime import date, datetime
from typing import Any

from app.models.models import parse_line_type

logger = logging.getLogger(__name__)

NS = {"ss": "urn:schemas-microsoft-com:office:spreadsheet"}

REQUIRED_COLUMNS = {
    "Order number",
    "Bedrijf",
    "Geplaatst op",
    "Gepland",
    "On-/Offnet",
}


@dataclass
class ParsedOrder:
    order_number: str
    bedrijf: str
    geplaatst_op: date
    gepland: date
    line_type: str


@dataclass
class ParseResult:
    orders: list[ParsedOrder] = field(default_factory=list)
    report_date: datetime | None = None
    warnings: list[str] = field(default_factory=list)
    skipped_rows: int = 0


def file_content_hash(content: bytes) -> str:
    return hashlib.sha256(content).hexdigest()


def _parse_date(value: str | None) -> date | None:
    if not value or not value.strip():
        return None
    value = value.strip()
    try:
        return datetime.fromisoformat(value.replace("Z", "+00:00")).date()
    except ValueError:
        pass
    for fmt, size in (
        ("%Y-%m-%dT%H:%M:%S.%f", 26),
        ("%Y-%m-%dT%H:%M:%S", 19),
        ("%Y-%m-%d %H:%M:%S", 19),
        ("%Y-%m-%d", 10),
    ):
        try:
            return datetime.strptime(value[:size], fmt).date()
        except ValueError:
            continue
    return None


def _cell_text(cell: ET.Element) -> str:
    data = cell.find("ss:Data", NS)
    if data is not None and data.text:
        return data.text.strip()
    return ""


def _row_cells(row: ET.Element) -> dict[int, str]:
    cells: dict[int, str] = {}
    col = 1
    for cell in row.findall("ss:Cell", NS):
        index_attr = cell.get(f"{{{NS['ss']}}}Index")
        if index_attr:
            col = int(index_attr)
        cells[col] = _cell_text(cell)
        col += 1
    return cells


def _find_worksheet(root: ET.Element, name: str) -> ET.Element | None:
    for ws in root.findall(".//ss:Worksheet", NS):
        if ws.get(f"{{{NS['ss']}}}Name") == name:
            return ws
    return None


def _parse_info_report_date(root: ET.Element) -> datetime | None:
    ws = _find_worksheet(root, "Info")
    if ws is None:
        return None
    table = ws.find("ss:Table", NS)
    if table is None:
        return None
    for row in table.findall("ss:Row", NS):
        cells = _row_cells(row)
        values = [cells.get(i, "") for i in sorted(cells.keys())]
        if len(values) >= 2 and values[0] == "Aangemaakt op":
            raw = values[1]
            for fmt in ("%Y-%m-%d %H:%M:%S", "%Y-%m-%dT%H:%M:%S"):
                try:
                    return datetime.strptime(raw[:19], fmt)
                except ValueError:
                    continue
    return None


def parse_excel_xml(content: bytes) -> ParseResult:
    result = ParseResult()
    try:
        root = ET.fromstring(content)
    except ET.ParseError as exc:
        raise ValueError(f"Ongeldig XML-bestand: {exc}") from exc

    result.report_date = _parse_info_report_date(root)

    ws = _find_worksheet(root, "Lijn orders")
    if ws is None:
        raise ValueError("Tabblad 'Lijn orders' niet gevonden in het bestand")

    table = ws.find("ss:Table", NS)
    if table is None:
        raise ValueError("Geen tabel gevonden op tabblad 'Lijn orders'")

    rows = table.findall("ss:Row", NS)
    if not rows:
        raise ValueError("Tabblad 'Lijn orders' bevat geen rijen")

    header_cells = _row_cells(rows[0])
    column_map: dict[str, int] = {name: idx for idx, name in header_cells.items()}

    missing = REQUIRED_COLUMNS - set(column_map.keys())
    if missing:
        raise ValueError(f"Ontbrekende kolommen: {', '.join(sorted(missing))}")

    for row_idx, row in enumerate(rows[1:], start=2):
        cells = _row_cells(row)

        def get_col(name: str) -> str:
            idx = column_map.get(name)
            return cells.get(idx, "") if idx else ""

        order_number = get_col("Order number")
        bedrijf = get_col("Bedrijf")
        geplaatst_raw = get_col("Geplaatst op")
        gepland_raw = get_col("Gepland")
        line_type_raw = get_col("On-/Offnet")

        if not order_number:
            result.skipped_rows += 1
            continue

        geplaatst_op = _parse_date(geplaatst_raw)
        gepland = _parse_date(gepland_raw)
        line_type = parse_line_type(line_type_raw)

        if not bedrijf or not geplaatst_op or not gepland or not line_type:
            result.warnings.append(f"Rij {row_idx} overgeslagen: ontbrekende verplichte velden voor order {order_number}")
            result.skipped_rows += 1
            continue

        result.orders.append(
            ParsedOrder(
                order_number=order_number,
                bedrijf=bedrijf,
                geplaatst_op=geplaatst_op,
                gepland=gepland,
                line_type=line_type.value,
            )
        )

    if not result.orders and result.skipped_rows > 0:
        result.warnings.append("Geen geldige orders geïmporteerd")

    logger.info("Parsed %d orders, skipped %d rows", len(result.orders), result.skipped_rows)
    return result
