import { useState } from "react";
import { useQuery } from "@tanstack/react-query";
import { AlertTriangle, ArrowRight, ChevronDown, ChevronUp, Search } from "lucide-react";
import {
  api,
  dashboardRowClass,
  formatDate,
  formatDateTime,
  formatSlaRiskLabel,
  sortCustomersBySlaDays,
  type CustomerCard,
  type OrderDetail,
  type SlaDaysSort,
} from "@/api/client";
import SlaDaysSortButtons from "@/components/SlaDaysSortButtons";

function ChangedDateCell({ order }: { order: OrderDetail }) {
  if (!order.is_changed_this_upload || !order.previous_gepland) {
    return <>{formatDate(order.new_gepland)}</>;
  }

  return (
    <div className="flex flex-wrap items-center gap-1.5">
      <span className="vz-cell-previous-date">{formatDate(order.previous_gepland)}</span>
      <ArrowRight size={14} className="text-ziggo shrink-0" />
      <span className={order.is_sla_risk ? "vz-cell-sla-overdue" : "vz-cell-changed-date"}>
        {formatDate(order.new_gepland)}
      </span>
      {order.days_shifted != null && (
        <span className="text-xs font-semibold text-ziggo dark:text-ziggo-200">
          (+{order.days_shifted} d)
        </span>
      )}
    </div>
  );
}

function CustomerCardComponent({
  customer,
  emphasizeChange,
}: {
  customer: CustomerCard;
  emphasizeChange?: boolean;
}) {
  const [expanded, setExpanded] = useState(
    customer.has_changed_this_upload || customer.has_sla_risk
  );

  const borderClass = customer.has_changed_this_upload
    ? "border-ziggo dark:border-ziggo ring-2 ring-ziggo/30"
    : customer.has_sla_risk
      ? "border-vodafone/40 dark:border-vodafone/30 ring-1 ring-vodafone/10"
      : "border-ziggo/60 dark:border-ziggo/50";

  return (
    <div
      className={`rounded-xl border bg-white dark:bg-neutral-900 ${borderClass} overflow-hidden ${
        emphasizeChange ? "shadow-md shadow-ziggo/10" : ""
      }`}
    >
      <button
        className={`w-full px-5 py-4 flex items-center justify-between text-left transition-colors ${
          customer.has_changed_this_upload
            ? "bg-ziggo-50/80 dark:bg-ziggo/10 hover:bg-ziggo-50 dark:hover:bg-ziggo/15"
            : "hover:bg-gray-50 dark:hover:bg-neutral-800/50"
        }`}
        onClick={() => setExpanded(!expanded)}
      >
        <div className="flex items-center gap-3">
          {customer.has_changed_this_upload ? (
            <span className="flex h-8 w-8 items-center justify-center rounded-full bg-ziggo text-white shrink-0">
              <ArrowRight size={16} />
            </span>
          ) : customer.has_sla_risk ? (
            <AlertTriangle className="text-vodafone shrink-0" size={20} />
          ) : null}
          <div>
            <h3 className="font-semibold text-base">{customer.bedrijf}</h3>
            <p className="text-sm text-gray-500 dark:text-gray-400">
              {customer.order_count} order{customer.order_count !== 1 ? "s" : ""}
              {customer.has_changed_this_upload && (
                <span className="ml-2 text-ziggo-800 dark:text-ziggo-100 font-semibold">
                  Gewijzigd in laatste upload
                </span>
              )}
              {!customer.has_changed_this_upload && customer.has_sla_risk && (
                <span className="ml-2 text-vodafone dark:text-vodafone-200 font-medium">SLA-risico</span>
              )}
            </p>
          </div>
        </div>
        {expanded ? <ChevronUp size={20} /> : <ChevronDown size={20} />}
      </button>
      {expanded && (
        <div className="border-t border-gray-200 dark:border-neutral-800 overflow-x-auto">
          <table className="w-full text-sm">
            <thead className="bg-gray-50 dark:bg-neutral-800/50 text-gray-500 dark:text-neutral-400">
              <tr>
                <th className="px-4 py-2 text-left">Order</th>
                <th className="px-4 py-2 text-left">Type</th>
                <th className="px-4 py-2 text-left">Geplaatst op</th>
                <th className="px-4 py-2 text-left">Gepland</th>
                <th className="px-4 py-2 text-left">SLA deadline</th>
                <th className="px-4 py-2 text-left">Status</th>
              </tr>
            </thead>
            <tbody>
              {customer.orders.map((o) => (
                <tr key={o.order_number} className={dashboardRowClass(o)}>
                  <td className="px-4 py-2 font-mono text-xs">{o.order_number}</td>
                  <td className="px-4 py-2 capitalize">{o.line_type}</td>
                  <td className="px-4 py-2">{formatDate(o.geplaatst_op)}</td>
                  <td className="px-4 py-2">
                    {o.is_changed_this_upload ? (
                      <ChangedDateCell order={o} />
                    ) : (
                      formatDate(o.new_gepland)
                    )}
                  </td>
                  <td className={`px-4 py-2 ${o.is_sla_risk ? "font-medium" : ""}`}>
                    {formatDate(o.sla_deadline)}
                  </td>
                  <td className="px-4 py-2">
                    {o.is_changed_this_upload && (
                      <span className="vz-badge-changed mr-1">Datum gewijzigd</span>
                    )}
                    {o.is_sla_risk ? (
                      <span className="vz-badge-sla">
                        {formatSlaRiskLabel(o.sla_days_over)}
                      </span>
                    ) : o.is_new_order ? (
                      <span className="vz-badge-new">Nieuw</span>
                    ) : (
                      !o.is_changed_this_upload && (
                        <span className="vz-badge-changed">Gewijzigd</span>
                      )
                    )}
                  </td>
                </tr>
              ))}
            </tbody>
          </table>
        </div>
      )}
    </div>
  );
}

function CustomerSection({
  title,
  subtitle,
  customers,
  emphasizeChange,
}: {
  title: string;
  subtitle?: string;
  customers: CustomerCard[];
  emphasizeChange?: boolean;
}) {
  if (customers.length === 0) return null;

  return (
    <section className="space-y-3">
      <div>
        <h3 className="text-lg font-semibold">{title}</h3>
        {subtitle && (
          <p className="text-sm text-gray-500 dark:text-neutral-400 mt-0.5">{subtitle}</p>
        )}
      </div>
      {customers.map((c) => (
        <CustomerCardComponent
          key={c.bedrijf}
          customer={c}
          emphasizeChange={emphasizeChange}
        />
      ))}
    </section>
  );
}

export default function DashboardPage() {
  const [search, setSearch] = useState("");
  const [slaFilter, setSlaFilter] = useState<"all" | "risk" | "changed">("all");
  const [slaDaysSort, setSlaDaysSort] = useState<SlaDaysSort>("default");

  const { data: dashboard, isLoading, error } = useQuery({
    queryKey: ["dashboard"],
    queryFn: api.getDashboard,
  });

  const { data: kpi } = useQuery({
    queryKey: ["kpi"],
    queryFn: api.getKpi,
  });

  if (isLoading) {
    return <div className="text-center py-20 text-gray-500">Laden...</div>;
  }

  if (error) {
    return (
      <div className="rounded-lg bg-vodafone-50 dark:bg-vodafone/10 border border-vodafone/20 dark:border-vodafone/30 p-4 text-vodafone-800 dark:text-vodafone-100">
        Fout bij laden: {(error as Error).message}
      </div>
    );
  }

  const filterCustomer = (c: CustomerCard) => {
    if (slaFilter === "risk" && !c.has_sla_risk) return false;
    if (slaFilter === "changed" && !c.has_changed_this_upload) return false;
    if (search && !c.bedrijf.toLowerCase().includes(search.toLowerCase())) {
      const matchOrder = c.orders.some((o) =>
        o.order_number.toLowerCase().includes(search.toLowerCase())
      );
      if (!matchOrder) return false;
    }
    return true;
  };

  const allCustomers = sortCustomersBySlaDays(
    (dashboard?.customers ?? []).filter(filterCustomer),
    slaDaysSort
  );

  const isFollowUpUpload = dashboard?.last_upload && !dashboard.last_upload.is_first_upload;
  const changedCustomers = allCustomers.filter((c) => c.has_changed_this_upload);
  const otherCustomers = allCustomers.filter((c) => !c.has_changed_this_upload);

  return (
    <div className="space-y-6">
      <div>
        <h2 className="vz-page-title">Dashboard</h2>
        <p className="vz-page-subtitle">
          Klanten met wijzigingen in geplande leverdata
        </p>
      </div>

      <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
        <div className="vz-card p-5">
          <p className="text-sm text-gray-500 dark:text-neutral-400">SLA-risico orders</p>
          <p className="text-3xl font-bold text-vodafone mt-1">
            {kpi?.sla_risk_count ?? 0}
          </p>
        </div>
        <div className="vz-card p-5">
          <p className="text-sm text-gray-500 dark:text-neutral-400">Wijzigingen (laatste upload)</p>
          <p className="text-3xl font-bold text-ziggo mt-1">{kpi?.changes_detected ?? 0}</p>
        </div>
        <div className="vz-card p-5">
          <p className="text-sm text-gray-500 dark:text-neutral-400">Laatste upload</p>
          <p className="text-sm font-medium mt-2">
            {dashboard?.last_upload
              ? formatDateTime(dashboard.last_upload.uploaded_at)
              : "Nog geen upload"}
          </p>
          {dashboard?.last_upload && (
            <p className="text-xs text-gray-400 mt-1">{dashboard.last_upload.filename}</p>
          )}
        </div>
      </div>

      <div className="flex flex-col lg:flex-row gap-3 lg:items-center lg:justify-between">
        <div className="flex flex-col sm:flex-row gap-3 flex-1">
          <div className="relative flex-1">
            <Search className="absolute left-3 top-1/2 -translate-y-1/2 text-gray-400" size={16} />
            <input
              type="text"
              placeholder="Zoek op bedrijf of order..."
              value={search}
              onChange={(e) => setSearch(e.target.value)}
              className="w-full pl-9 pr-4 py-2 vz-input"
            />
          </div>
          <select
            value={slaFilter}
            onChange={(e) => setSlaFilter(e.target.value as "all" | "risk" | "changed")}
            className="px-4 py-2 vz-input"
          >
            <option value="all">Alle relevante orders</option>
            <option value="changed">Alleen wijzigingen laatste upload</option>
            <option value="risk">Alleen SLA-risico</option>
          </select>
        </div>
        <SlaDaysSortButtons value={slaDaysSort} onChange={setSlaDaysSort} />
      </div>

      {!dashboard?.last_upload ? (
        <div className="text-center py-16 rounded-xl border border-dashed border-ziggo/40 dark:border-ziggo/30">
          <p className="text-gray-500">Upload een dagrapport om te beginnen.</p>
        </div>
      ) : allCustomers.length === 0 ? (
        <div className="text-center py-16 vz-card">
          <p className="text-gray-500">Geen klanten met relevante wijzigingen.</p>
        </div>
      ) : isFollowUpUpload ? (
        <div className="space-y-8">
          <CustomerSection
            title={`Gewijzigd in laatste upload (${changedCustomers.length})`}
            subtitle="Orders waar Gepland is verschoven ten opzichte van de vorige upload"
            customers={changedCustomers}
            emphasizeChange
          />
          <CustomerSection
            title={`Overig SLA-risico (${otherCustomers.length})`}
            subtitle="Geen datumwijziging in de laatste upload, wel boven SLA-deadline"
            customers={otherCustomers}
          />
        </div>
      ) : (
        <div className="space-y-3">
          {allCustomers.map((c) => (
            <CustomerCardComponent key={c.bedrijf} customer={c} />
          ))}
        </div>
      )}
    </div>
  );
}
