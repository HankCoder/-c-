#!/bin/sh

valgrind --tool=memcheck --leak-check=yes -v ./mime_qp
