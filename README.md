# CrossFactionResFix

A lightweight 32-bit DLL for World of Warcraft 1.12.1 that enables cross-faction resurrection attempts on released players without modifying `WoW.exe` on disk.

Developed and tested with [OctoWoW](https://octowow.st/) and [VanillaFixes](https://github.com/hannesmann/vanillafixes). This is an unofficial community project.

## Features

- Applies the cross-faction released-corpse resurrection patch **in memory**, leaving `WoW.exe` untouched for launcher updates.
- Works with VanillaFixes' `dlls.txt` loader.
- Checks the surrounding machine instructions before patching; incompatible executables are left unchanged.
- Emits status messages via Windows `OutputDebugStringA`.
- Does not change server-side resurrection rules.

## Installation

**Requirements:** A compatible 32-bit WoW 1.12.1 executable and VanillaFixes with DLL loading enabled.

1. Download `CrossFactionResFix.dll` from `bin/` (or a published release).
2. Copy it to the WoW installation folder alongside `WoW.exe`.
3. Open `dlls.txt` in the same folder and add this line, keeping the existing entries:

   ```text
   CrossFactionResFix.dll
   ```

4. Start the game through VanillaFixes (or a launcher configured to use it).

Do **not** replace or manually patch your `WoW.exe`.

## How it works

For the tested OctoWoW executable, the DLL looks for a specific instruction sequence in the main module at **RVA `0x2067DE`** and changes the comparison immediate from `04` to `00`. It uses `VirtualProtect`, `FlushInstructionCache`, and restores the previous memory protection. If the signature or byte differs from the expected values, it skips the patch. If the byte is already `00`, it reports that the patch is already applied.

This is **not** a general-purpose patch for every WoW 1.12.1 build. The RVA and signature are build-dependent; a matching signature is a safety check, not proof of universal compatibility.

## Compatibility and tests

| Scenario | Status |
| --- | --- |
| OctoWoW on Linux/Wine with VanillaFixes | Tested working |
| Alliance resurrecting a released Horde player outside party | Tested working |
| Horde resurrecting a released Alliance player outside party | Not tested |
| Resurrection without spirit release, outside party | Reported as not permitted |
| Resurrection without spirit release, inside party | Reported as permitted |
| Native Windows | Not separately tested |
| Other WoW 1.12.1 clients | Not verified |

Server-side restrictions still apply. Client or launcher updates may change the patch location or surrounding code.

## Debugging

The DLL uses `OutputDebugStringA` rather than writing a log file. Under Wine, run the game with the `WINEDEBUG=+debugstr` environment variable, then look for `CrossFactionResFix` messages.

Possible messages:

```text
CrossFactionResFix: patch applied in memory
CrossFactionResFix: already applied
CrossFactionResFix: opcode mismatch (prefix), no patch
CrossFactionResFix: opcode mismatch (suffix), no patch
CrossFactionResFix: unexpected target byte, no patch
CrossFactionResFix: VirtualProtect failed
```

## Build from source

On Ubuntu, install MinGW-w64 and compile the DLL for **32-bit Windows**:

```bash
sudo apt install gcc-mingw-w64-i686
i686-w64-mingw32-gcc -Os -shared -s -o CrossFactionResFix.dll src/CrossFactionResFix.c
```

The source is self-contained and uses Windows system APIs. The checked-in `bin/CrossFactionResFix.dll` is the original binary tested with OctoWoW; a rebuild may not be byte-for-byte identical.

## Credits

The underlying one-byte correction is from [Vanilla Tweaks](https://github.com/tubtubs/vanilla-tweaks), originally created by [brndd](https://github.com/brndd/vanilla-tweaks). The [OctoWoW Modernization Tool](https://octowow.st/git/Dusk/Modernization-Tool) also exposes this fix. [VanillaFixes](https://github.com/hannesmann/vanillafixes) provides DLL loading.

This project repackages the correction as a standalone runtime DLL.

## License

MIT — see [LICENSE](LICENSE). Not affiliated with Blizzard Entertainment or the OctoWoW team.
