import { useEffect, useState } from "react";
import { useMutation, useQuery, useQueryClient } from "@tanstack/react-query";
import { Save, CheckCircle } from "lucide-react";
import { api } from "@/api/client";

export default function SettingsPage() {
  const queryClient = useQueryClient();
  const { data: settings, isLoading } = useQuery({
    queryKey: ["settings"],
    queryFn: api.getSettings,
  });

  const [onnet, setOnnet] = useState(30);
  const [offnet, setOffnet] = useState(45);
  const [special, setSpecial] = useState(60);
  const [retention, setRetention] = useState(365);
  const [saved, setSaved] = useState(false);

  useEffect(() => {
    if (settings) {
      setOnnet(settings.sla_days.onnet || 30);
      setOffnet(settings.sla_days.offnet || 45);
      setSpecial(settings.sla_days.special || 60);
      setRetention(settings.retention_days);
    }
  }, [settings]);

  const slaMutation = useMutation({
    mutationFn: () => api.updateSla({ onnet, offnet, special }),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["settings"] });
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    },
  });

  const retentionMutation = useMutation({
    mutationFn: () => api.updateRetention(retention),
    onSuccess: () => {
      queryClient.invalidateQueries({ queryKey: ["settings"] });
      setSaved(true);
      setTimeout(() => setSaved(false), 3000);
    },
  });

  if (isLoading) {
    return <div className="text-center py-20 text-gray-500">Laden...</div>;
  }

  return (
    <div className="max-w-2xl space-y-8">
      <div>
        <h2 className="text-2xl font-bold">Instellingen</h2>
        <p className="text-gray-500 dark:text-gray-400 mt-1">
          Configureer SLA-waarden en bewaartermijn
        </p>
      </div>

      {saved && (
        <div className="flex items-center gap-2 text-green-600 dark:text-green-400 text-sm">
          <CheckCircle size={16} />
          Instellingen opgeslagen
        </div>
      )}

      {!settings?.sla_configured && (
        <div className="rounded-lg bg-amber-50 dark:bg-amber-900/20 border border-amber-200 dark:border-amber-800 p-4 text-amber-800 dark:text-amber-200 text-sm">
          SLA-dagen zijn nog niet geconfigureerd. Stel de waarden in voordat u rapporten uploadt.
        </div>
      )}

      <section className="rounded-xl border border-gray-200 dark:border-gray-800 bg-white dark:bg-gray-900 p-6 space-y-5">
        <h3 className="font-semibold text-lg">SLA werkdagen per lijn type</h3>
        <p className="text-sm text-gray-500">
          Berekend vanaf Geplaatst op, exclusief weekenden en Nederlandse feestdagen.
        </p>
        <div className="grid grid-cols-1 sm:grid-cols-3 gap-4">
          {[
            { label: "Onnet", value: onnet, set: setOnnet },
            { label: "Offnet (Nearnet)", value: offnet, set: setOffnet },
            { label: "Special", value: special, set: setSpecial },
          ].map(({ label, value, set }) => (
            <div key={label}>
              <label className="block text-sm font-medium mb-1">{label}</label>
              <input
                type="number"
                min={1}
                max={999}
                value={value}
                onChange={(e) => set(Number(e.target.value))}
                className="w-full px-3 py-2 rounded-lg border border-gray-200 dark:border-gray-700 bg-white dark:bg-gray-900 text-sm"
              />
              <span className="text-xs text-gray-400 mt-1 block">werkdagen</span>
            </div>
          ))}
        </div>
        <button
          onClick={() => slaMutation.mutate()}
          disabled={slaMutation.isPending}
          className="flex items-center gap-2 px-4 py-2 rounded-lg bg-brand-600 text-white text-sm font-medium hover:bg-brand-700 disabled:opacity-50"
        >
          <Save size={16} />
          SLA opslaan
        </button>
      </section>

      <section className="rounded-xl border border-gray-200 dark:border-gray-800 bg-white dark:bg-gray-900 p-6 space-y-5">
        <h3 className="font-semibold text-lg">Bewaartermijn wijzigingslog</h3>
        <div>
          <label className="block text-sm font-medium mb-1">Retentie (dagen)</label>
          <input
            type="number"
            min={30}
            max={3650}
            value={retention}
            onChange={(e) => setRetention(Number(e.target.value))}
            className="w-full max-w-xs px-3 py-2 rounded-lg border border-gray-200 dark:border-gray-700 bg-white dark:bg-gray-900 text-sm"
          />
        </div>
        <button
          onClick={() => retentionMutation.mutate()}
          disabled={retentionMutation.isPending}
          className="flex items-center gap-2 px-4 py-2 rounded-lg bg-brand-600 text-white text-sm font-medium hover:bg-brand-700 disabled:opacity-50"
        >
          <Save size={16} />
          Retentie opslaan
        </button>
      </section>
    </div>
  );
}
