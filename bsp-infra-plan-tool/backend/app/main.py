import logging
from contextlib import asynccontextmanager
from datetime import datetime, timedelta, timezone

from apscheduler.schedulers.asyncio import AsyncIOScheduler
from fastapi import FastAPI
from fastapi.middleware.cors import CORSMiddleware
from sqlalchemy import delete

from app.api.routes import router
from app.config import settings
from app.database import Base, SessionLocal, engine
from app.models.models import OrderChange
from app.services.settings_service import get_retention_days

logging.basicConfig(
    level=getattr(logging, settings.log_level.upper(), logging.INFO),
    format="%(asctime)s %(levelname)s [%(name)s] %(message)s",
)
logger = logging.getLogger(__name__)

scheduler = AsyncIOScheduler()


async def cleanup_old_changes():
    async with SessionLocal() as session:
        retention = await get_retention_days(session)
        cutoff = datetime.now(timezone.utc) - timedelta(days=retention)
        await session.execute(delete(OrderChange).where(OrderChange.created_at < cutoff))
        await session.commit()
        logger.info("Retention cleanup completed (cutoff: %s)", cutoff.isoformat())


@asynccontextmanager
async def lifespan(app: FastAPI):
    async with engine.begin() as conn:
        await conn.run_sync(Base.metadata.create_all)
    scheduler.add_job(cleanup_old_changes, "cron", hour=2, minute=0)
    scheduler.start()
    logger.info("Application started")
    yield
    scheduler.shutdown()


app = FastAPI(
    title="BSP Infrastructure Plan Tool",
    version="1.0.0",
    lifespan=lifespan,
)

app.add_middleware(
    CORSMiddleware,
    allow_origins=settings.cors_origin_list,
    allow_credentials=True,
    allow_methods=["*"],
    allow_headers=["*"],
)

app.include_router(router)


@app.get("/api/health")
async def health():
    return {"status": "ok"}
