```bash
#mettre le dossier ou les commandes seront executee en chmod 777
#cree une seed et sort tout en test.*
docker run --rm -v "$PWD:/work" wil42/ecomesh:1.0 generate --seed 1001 --out test
```
```bash
#lance une simu sur test, logs_out contiendra les echanges couts etc, sur les lignes E
docker run --rm -v "$PWD:/work" wil42/ecomesh:1.0 simulate test logs_out
```

