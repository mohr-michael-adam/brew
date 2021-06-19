VERSION := $(shell cat VERSION)

.PHONY: build
build:
	rm -rf packages

	mkdir -p packages

	docker build -t pip-installer .

	docker run --rm -v $(shell pwd)/packages:/packages pip-installer pip3 install --target ./packages -r requirements.txt

	cd packages; zip -r ../../../bin/receiver-$(VERSION).zip .

	zip -g ../../bin/receiver-$(VERSION).zip receiver.py
	zip -g ../../bin/receiver-$(VERSION).zip sign_pub.key