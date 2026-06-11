from pydantic_settings import BaseSettings, SettingsConfigDict


class Settings(BaseSettings):
    model_config = SettingsConfigDict(env_file=".env", extra="ignore")

    database_url: str = "postgresql+asyncpg://bsp:bsp@db:5432/bsp_infra"
    cors_origins: str = "http://localhost:5173,http://localhost:8080"
    log_level: str = "INFO"
    retention_days: int = 365
    max_upload_size_mb: int = 50

    @property
    def cors_origin_list(self) -> list[str]:
        return [o.strip() for o in self.cors_origins.split(",") if o.strip()]


settings = Settings()
