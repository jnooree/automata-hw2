#!/bin/bash -eu

prefix='Automata_HW2_2022-28022'

rm -rf test/generated
scripts/generate_test.py "test/generated"

mv test/generated .
rm -rf test
mv generated test

cp scripts/compile.sh "${prefix}_compile.sh"

run_sh="${prefix}_run.sh"
echo "#!/bin/sh
bin/run" >>"$run_sh"
chmod +x "$run_sh"

zip -r "${prefix}.zip" ./*.{sh,pdf} CMakeLists.txt src test
