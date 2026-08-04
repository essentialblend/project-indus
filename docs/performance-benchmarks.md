# Performance notes

I wanted performance numbers for Indus that I could back, rather than timings taken from the preview window. To that end, I added a small headless mode that runs the real renderer and prints the render time, worker count, ray count, primitive-test count, and a hash of the final image.

## Test setup

- Date: 4 August 2026
- CPU: AMD Ryzen 7 2700X (8 cores, 16 logical processors)
- Build: `Release|x64` with `/O2` and `/GL`
- Scene: the same fixed-seed, procedurally generated 480-primitive scene in every comparison

For the shorter BVH comparison, I alternate the two configurations, run each one five times, and use the median rather than the best result. The direct 1080p threading comparison is one matched pair because the single-worker render takes more than 21 minutes. I also keep a smaller repeated threading suite as a repeatability check. The fixed scene and sampler keep the workload stable. The executable prints ray counts, primitive-test counts, and an FNV-1a hash of the final 8-bit RGBA image, which helps me catch cases where supposedly equivalent runs did different work or produced different output.

The timer includes path integration, progressive snapshot generation, and the final film conversion performed by the integrator. It does not include the SFML interface, PNG encoding, disk I/O, or the post-render image hash.

## Single-threaded versus multithreaded rendering

This comparison measures the custom `std::jthread` pool and tiled sample-wave scheduler at 1920x1080 and 6 spp. Since the stratified sampler produces square sample counts, `--spp 6` selects the engine's existing independent sampler. The render still uses the normal Gaussian High filter with jitter enabled.

```powershell
.\x64\Release\indus.exe --benchmark --threads 1  --aggregate bvh --resolution 1920x1080 --spp 6
.\x64\Release\indus.exe --benchmark --threads 16 --aggregate bvh --resolution 1920x1080 --spp 6 --write-image
```

The thread pool reserves one logical processor as headroom, so `--threads 16` creates 15 render workers.

| Configuration | Render time | Rays | Rays/second | Primitive tests | Image hash |
| --- | ---: | ---: | ---: | ---: | --- |
| 1 worker | 1,183.776 s | 31,245,531 | 26,395 | 150,592,726 | `59eaf126df0da694` |
| 15 workers | 127.012 s | 31,245,531 | 246,005 | 150,592,726 | `59eaf126df0da694` |

The 15-worker render is **9.32x faster** and takes **89.3% less time**. Ray throughput increases from about 26.4K to 246.0K rays/second. Both runs trace the same 31.2 million rays, perform the same primitive tests, and produce the same final image hash.

The written 15-worker image is `renders/indus_16x9_1080p_spp6_2m07s_TJ8V5D.png`. PNG encoding happens after the benchmark timer stops. At 6 spp the image is still visibly noisy, but I found no tearing, missing tiles, or other obvious concurrency corruption.

As a repeatability check, a five-pair 320x180/4-spp suite measured a 9.13x median speedup, from 22.032 s to 2.414 s, with matching counters and hashes in every pair. A separate 160x90/1-spp check also produced the same hash with 1, 2, 4, 8, and 15 workers.

### Note about the Gaussian accumulation fix

The benchmark initially exposed inconsistent multithreaded image hashes. The filter samples a continuous position around the pixel being evaluated, but I was passing that continuous position back to `FilmBase::addSample()` as the destination. That allowed neighboring tiles to write into the same pixel.

`addSample()` now takes the discrete destination pixel explicitly. The continuous filter-sampled position is still used to generate the camera ray, while the resulting estimate is accumulated into the pixel being evaluated. The Gaussian results above were collected after that correction.

### Note about the coated-diffuse mixture fix

The coated-diffuse sampler uses the first uniform variate to select either the GGX coat or Lambertian base. It now remaps the selected probability interval back to `[0,1)` before passing that variate to the chosen lobe sampler. This makes the generated coat/base mixture agree with the weighted mixture PDF used by the path integrator. Every result on this page was rerun after that correction.

## SAH BVH versus linear traversal

For this comparison I keep the render single-threaded and change only the root aggregate. The primitive list checks every primitive for each ray. The BVH uses the existing flattened, surface-area-heuristic hierarchy.

```powershell
.\x64\Release\indus.exe --benchmark --threads 1 --aggregate bvh  --resolution 64x36 --strata 1
.\x64\Release\indus.exe --benchmark --threads 1 --aggregate list --resolution 64x36 --strata 1
```

| Accelerator | Run times (ms) | Median | Rays | Primitive tests | Tests/ray | Image hash |
| --- | --- | ---: | ---: | ---: | ---: | --- |
| Flattened SAH BVH | 213; 213; 212; 211; 210 | 212 ms | 5,783 | 27,879 | 4.82 | `ee7f5c89319aea86` |
| Primitive list | 21,201; 19,423; 19,229; 19,013; 19,143 | 19,229 ms | 5,783 | 2,775,840 | 480.00 | `ee7f5c89319aea86` |

In this snapshot, the BVH is **90.7x faster** and performs **99.0% fewer primitive tests**. Both paths trace the same 5,783 rays and produce the same image hash in every pair.
