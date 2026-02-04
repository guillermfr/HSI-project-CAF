VENV = .venv
PYTHON_VENV = $(VENV)/bin/python3
OUTPUT_DIR = output
LIB_NAME = lib.a

all: run build-lib

# 1. Lance la génération des fichiers
run: $(PYTHON_VENV)
	$(PYTHON_VENV) generation_lib.py

build-lib: run
	@echo "Compiling generated files..."
	gcc -c $(OUTPUT_DIR)/*.c -I$(OUTPUT_DIR)
	@echo "Création de la librairie statique $(LIB_NAME)..."
	ar rcs $(LIB_NAME) *.o
	@rm *.o
	@echo "$(LIB_NAME) est prêt."

$(PYTHON_VENV):
	python3 -m venv $(VENV)
	$(PYTHON_VENV) -m pip install --upgrade pip
	$(PYTHON_VENV) -m pip install -r requirements.txt

clean:
	rm -rf $(VENV)
	rm -rf $(OUTPUT_DIR)
	rm -rf __pycache__
	rm -f *.o $(LIB_NAME) $(HEADER_NAME)

.PHONY: all run build-lib clean