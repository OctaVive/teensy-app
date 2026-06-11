import json
import logging

from sqlalchemy import select
from sqlalchemy.ext.asyncio import AsyncSession

from app.models.models import AppSetting, LineType

logger = logging.getLogger(__name__)

SLA_SETTING_KEY = "sla_days"
RETENTION_SETTING_KEY = "retention_days"

DEFAULT_SLA = {
    LineType.ONNET.value: 0,
    LineType.OFFNET.value: 0,
    LineType.SPECIAL.value: 0,
}


async def get_setting(session: AsyncSession, key: str) -> str | None:
    result = await session.execute(select(AppSetting).where(AppSetting.key == key))
    row = result.scalar_one_or_none()
    return row.value if row else None


async def set_setting(session: AsyncSession, key: str, value: str) -> None:
    result = await session.execute(select(AppSetting).where(AppSetting.key == key))
    row = result.scalar_one_or_none()
    if row:
        row.value = value
    else:
        session.add(AppSetting(key=key, value=value))


async def get_sla_days(session: AsyncSession) -> dict[str, int]:
    raw = await get_setting(session, SLA_SETTING_KEY)
    if not raw:
        return dict(DEFAULT_SLA)
    data = json.loads(raw)
    return {k: int(v) for k, v in data.items()}


async def set_sla_days(session: AsyncSession, sla: dict[str, int]) -> None:
    await set_setting(session, SLA_SETTING_KEY, json.dumps(sla))


async def get_retention_days(session: AsyncSession) -> int:
    raw = await get_setting(session, RETENTION_SETTING_KEY)
    if not raw:
        from app.config import settings
        return settings.retention_days
    return int(raw)


async def set_retention_days(session: AsyncSession, days: int) -> None:
    await set_setting(session, RETENTION_SETTING_KEY, str(days))


def sla_configured(sla_days: dict[str, int]) -> bool:
    return all(sla_days.get(lt.value, 0) > 0 for lt in LineType)
