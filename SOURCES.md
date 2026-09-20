# Embodied carbon dataset — sources

`boavizta_impact_data.csv` is a small, pinned snapshot of embodied-carbon figures
for the hardware classes an EcoMesh node and gateway are built from, derived from
Boavizta's open environmental-footprint data.

- Upstream: Boavizta open data — https://github.com/Boavizta/environmental-footprint-data
  and the Boavizta API (https://doc.api.boavizta.org).
- Snapshot pinned for the whole promotion so results are stable across the
  semester. Do not silently update it mid-semester.

## What each column means

- `component` — the part it stands for in an EcoMesh node/gateway.
- `proxy_entry` — the Boavizta-class figure actually used.
- `embodied_gco2e` — manufacturing footprint, grams CO2e.
- `expected_lifetime_years` — nominal service life used for amortisation.
- `error_margin_pct` — how rough the proxy is.
- `source_note` — why this proxy was chosen.

## Deliberate limitation

Boavizta's open data has **no entries specific to microcontrollers, small
Li-ion cells, or serial flash**. Every figure here is therefore a documented
**approximation**: the closest available class, scaled where needed, with an
error margin. This is not a gap to "fix"; incomplete embodied data is the normal
condition of the field. Understanding, justifying, and where possible refining
these approximations is part of the project.
