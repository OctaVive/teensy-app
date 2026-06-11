import pytest
from datetime import date

from app.services.sla import add_business_days, calculate_sla_deadline, is_sla_risk


def test_add_business_days_skips_weekend():
    # Monday 2026-06-08 + 5 business days = Friday 2026-06-13
    start = date(2026, 6, 8)
    assert add_business_days(start, 5) == date(2026, 6, 15)


def test_sla_deadline():
    start = date(2026, 1, 2)  # Friday
    deadline = calculate_sla_deadline(start, 10)
    assert deadline is not None
    assert deadline > start


def test_is_sla_risk():
    deadline = date(2026, 6, 30)
    assert is_sla_risk(date(2026, 7, 1), deadline) is True
    assert is_sla_risk(date(2026, 6, 30), deadline) is False
    assert is_sla_risk(date(2026, 6, 29), deadline) is False


def test_is_sla_risk_none_deadline():
    assert is_sla_risk(date(2026, 7, 1), None) is False
