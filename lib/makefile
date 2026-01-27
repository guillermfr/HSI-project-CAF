VENV = .venv
PYTHON_VENV = $(VENV)/bin/python3

all: run

run: $(PYTHON_VENV)
	$(PYTHON_VENV) generation_lib.py


$(PYTHON_VENV):
	python3 -m venv $(VENV)
	$(PYTHON_VENV) -m pip install --upgrade pip
	$(PYTHON_VENV) -m pip install -r requirements.txt

clean:
	rm -rf $(VENV)
	rm -rf output
	rm -rf __pycache__