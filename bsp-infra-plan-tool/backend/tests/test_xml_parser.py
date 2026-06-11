import pytest
from datetime import date

from app.models.models import parse_line_type
from app.services.xml_parser import parse_excel_xml


MINIMAL_XML = b"""<?xml version='1.0' encoding='utf-8'?>
<ss:Workbook xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet">
 <ss:Worksheet ss:Name="Info">
  <ss:Table>
   <ss:Row>
    <ss:Cell><ss:Data ss:Type="String">Aangemaakt op</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">2026-06-11 10:18:45</ss:Data></ss:Cell>
   </ss:Row>
  </ss:Table>
 </ss:Worksheet>
 <ss:Worksheet ss:Name="Lijn orders">
  <ss:Table>
   <ss:Row>
    <ss:Cell><ss:Data ss:Type="String">Bedrijf</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">Order number</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">Geplaatst op</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">Gepland</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">On-/Offnet</ss:Data></ss:Cell>
   </ss:Row>
   <ss:Row>
    <ss:Cell><ss:Data ss:Type="String">Test BV</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">VF-2025-L001</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="DateTime">2026-01-15T00:00:00.000</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="DateTime">2026-06-30T00:00:00.000</ss:Data></ss:Cell>
    <ss:Cell><ss:Data ss:Type="String">Onnet</ss:Data></ss:Cell>
   </ss:Row>
  </ss:Table>
 </ss:Worksheet>
</ss:Workbook>"""


def test_parse_line_type():
    assert parse_line_type("Onnet").value == "onnet"
    assert parse_line_type("Offnet").value == "offnet"
    assert parse_line_type("Special").value == "special"
    assert parse_line_type("") is None


def test_parse_minimal_xml():
    result = parse_excel_xml(MINIMAL_XML)
    assert len(result.orders) == 1
    assert result.orders[0].order_number == "VF-2025-L001"
    assert result.orders[0].bedrijf == "Test BV"
    assert result.orders[0].geplaatst_op == date(2026, 1, 15)
    assert result.orders[0].gepland == date(2026, 6, 30)
    assert result.report_date is not None


def test_missing_sheet_raises():
    bad = b"""<?xml version='1.0'?><ss:Workbook xmlns:ss="urn:schemas-microsoft-com:office:spreadsheet"></ss:Workbook>"""
    with pytest.raises(ValueError, match="Lijn orders"):
        parse_excel_xml(bad)
