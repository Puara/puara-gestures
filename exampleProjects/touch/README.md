# Mac instructions to instal python dependencies after cloning
- `python3 -m venv .venv` (`py -m venv .venv` on win) to create the `.venv` folder (this can take a while)
- `source .venv/bin/activate` to activate the `venv`
    - or potentially `source .venv/Scripts/activate` on windoze
	- after this, `where python3` (`where python` on win) should return a python inside the `.venv` folder
- `python3 -m pip install --upgrade pip`
- `python3 -m pip install -r requirements.txt`
- to deactivate the venv: `deactivate`

# Windows instructions to instal python dependencies after cloning
- `python -m venv .venv` to create the `.venv` folder (this can take a while)
- `source .venv/Scripts/activate` to activate the `venv`
    - after this, `where python` should return a path inside the `.venv` folder
- `python -m pip install --upgrade pip`
- `python -m pip install -r requirements.txt`
- to deactivate the venv: `deactivate`

After doing either of the above, VSCode should "just work" ™️