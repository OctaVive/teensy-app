# Implementation Plan — BSP Infrastructure Plan Tool

## Milestones

| # | Milestone | Deliverables |
|---|-----------|--------------|
| M1 | Project scaffold | Docker Compose, backend/frontend skeleton, README |
| M2 | Data layer | SQLAlchemy models, Alembic migrations, settings store |
| M3 | XML ingestion | Parser, upload API, duplicate detection |
| M4 | Business logic | SLA calculator, change detection, order lifecycle |
| M5 | REST API | Dashboard, changes, settings endpoints |
| M6 | Frontend | Upload, dashboard, history, settings pages |
| M7 | Polish | Dark mode, CSV export, retention job, Dutch labels |

## Development phases

### Phase A — Backend core (M1–M4)

1. Initialize FastAPI app with async SQLAlchemy
2. Implement models and auto-migration on startup
3. Build SpreadsheetML parser with unit tests against sample structure
4. Implement SLA service with NL business days
5. Implement report comparison pipeline

### Phase B — API & integration (M5)

1. Wire upload endpoint with transaction-safe processing
2. Dashboard aggregation grouped by `Bedrijf`
3. Change history with pagination and filters
4. Settings CRUD for SLA and retention

### Phase C — Frontend (M6–M7)

1. Vite + React + TypeScript + Tailwind setup
2. API client with React Query
3. Pages: Dashboard, Upload, Geschiedenis, Instellingen
4. Dark mode toggle, responsive customer cards
5. CSV export button on change history

## Testing strategy

| Layer | Approach |
|-------|----------|
| Unit | SLA calculator, date parsing, change detection logic |
| Integration | Upload pipeline with in-memory SQLite / test PostgreSQL |
| API | FastAPI TestClient for all endpoints |
| Frontend | Manual E2E; component tests for dashboard cards (optional v1) |
| Sample data | Use anonymized Excel-XML structure from FD |

### Key test cases

- First upload marks all orders new
- Second upload detects `Gepland` moved later only
- SLA risk flagged when new date exceeds deadline
- Removed orders silently completed
- Duplicate upload rejected
- Lenient parse skips invalid rows

## Deployment strategy

1. Clone repository to on-prem server
2. Copy `.env.example` → `.env`, set database credentials
3. `docker compose up -d --build`
4. Open browser at configured port (default 8080)
5. Configure SLA days in Instellingen before first meaningful upload
6. Upload daily Excel-XML reports manually

### Rollback

- `docker compose down`
- Restore PostgreSQL volume from manual backup if needed (no automated backup v1)

## Post-v1 roadmap

- Azure AD authentication
- Actual delivery SLA KPI when data available
- Email/Teams notifications
- SFTP automated pickup
- Additional KPI dashboard widgets
