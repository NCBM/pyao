#include <Python.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ao/ao.h>

#define AO_DEVICE_SIZE 84

// ao_device descriptor
ao_device **ao_device_list = NULL;
int ao_device_count = 0;

static PyObject* pyao_init(PyObject* self) {
    ao_initialize();
    Py_RETURN_NONE;
}

static PyObject* pyao_shutdown(PyObject* self) {
    ao_shutdown();
    Py_RETURN_NONE;
}

static PyObject* pyao_default_driver_id(PyObject* self) {
    int drvid = ao_default_driver_id();
    return Py_BuildValue("i", drvid);
}

int add_ao_device(ao_device *device) {
    if (device == NULL) {
        return -1;
    }
    ao_device_list = (ao_device **)realloc(ao_device_list, AO_DEVICE_SIZE * (ao_device_count + 1));
    ao_device_list[ao_device_count] = device;
    return ao_device_count++;
}

static PyObject* pyao_open_live(PyObject* self, PyObject* args, PyObject* kwargs) {
    int drvid;
    ao_device* device;
    ao_sample_format aofmt;
    memset(&aofmt, 0, sizeof(aofmt));
    PyObject* pyfmt;

    static char* argsname[] = {"driver", "bits", "chs", "rate", "bfmt", "matrix", NULL};
    if (!PyArg_ParseTupleAndKeywords(
                args, kwargs, "iiiiis:pyao_open_live", argsname,
                &drvid, &aofmt.bits, &aofmt.channels, &aofmt.rate, &pyfmt, &aofmt.matrix
                ))
        return NULL;

    device = ao_open_live(drvid, &aofmt, NULL);
    if (device == NULL) {
        PyErr_SetString(PyExc_OSError, "Unable to open an audio device");
        return NULL;
    }
    int d_index = add_ao_device(device);
    return Py_BuildValue("i", d_index);
}

static PyObject* pyao_open_file(PyObject* self, PyObject* args, PyObject* kwargs) {
    int drvid, ow = 0;
    ao_device* device;
    ao_sample_format aofmt;
    memset(&aofmt, 0, sizeof(aofmt));
    PyObject* pyfmt;
    char* filename;

    static char* argsname[] = {"driver", "filename", "bits", "chs", "rate", "bfmt", "matrix", "overwrite", NULL};
    if (!PyArg_ParseTupleAndKeywords(
                args, kwargs, "isiiiisi:pyao_open_file", argsname,
                &drvid, &filename, &aofmt.bits, &aofmt.channels, &aofmt.rate, &pyfmt, &aofmt.matrix, &ow
                ))
        return NULL;

    device = ao_open_file(drvid, filename, ow, &aofmt, NULL);
    if (device == NULL) {
        PyErr_SetString(PyExc_OSError, "Unable to open an audio device");
        return NULL;
    }
    int d_index = add_ao_device(device);
    return Py_BuildValue("i", d_index);
}

static PyObject* pyao_close(PyObject* self, PyObject* args) {
    int d_index;
    if (!PyArg_ParseTuple(args, "i:pyao_close", &d_index))
        return NULL;

    ao_device* device = ao_device_list[d_index];
    int code = ao_close(device);
    return Py_BuildValue("i", code);
}

static PyObject* pyao_play(PyObject* self, PyObject* args, PyObject* kwargs) {
    int device;
    char* bytes;
    uint_32 size;

    static char* argsname[] = {"device", "data", NULL};
    if (!PyArg_ParseTupleAndKeywords(args, kwargs, "iy#:pyao_play", argsname, &device, bytes, &size))
        return NULL;

    ao_device* _device = ao_device_list[device];
    int code = ao_play(_device, bytes, size);
    return Py_BuildValue("i", code);
}

static PyMethodDef _methods[] = {
    {"pyao_init",               (PyCFunction)pyao_init,                 METH_NOARGS,                    "pyao_init()\n--\n\nInitialize the audio library."},
    {"pyao_shutdown",           (PyCFunction)pyao_shutdown,             METH_NOARGS,                    "pyao_shutdown()\n--\n\nShutdown the audio library."},
    {"pyao_default_driver_id",  (PyCFunction)pyao_default_driver_id,    METH_NOARGS,                    "pyao_default_driver_id()\n--\n\nGet the default audio driver ID."},
    {"pyao_open_live",          (PyCFunction)pyao_open_live,            METH_VARARGS | METH_KEYWORDS,   "pyao_open_live(driver, bits, chs, rate, bfmt, matrix)\n--\n\nOpen an audio device for live playback."},
    {"pyao_open_file",          (PyCFunction)pyao_open_file,            METH_VARARGS | METH_KEYWORDS,   "pyao_open_file(driver, filename, bits, chs, rate, bfmt, matrix, overwrite)\n--\n\nOpen an audio device for file playback."},
    {"pyao_close",              (PyCFunction)pyao_close,                METH_VARARGS,                   "pyao_close(device)\n--\n\nClose an audio device."},
    {"pyao_play",               (PyCFunction)pyao_play,                 METH_VARARGS | METH_KEYWORDS,   "pyao_play(device, data)\n--\n\nPlay a buffer on an audio device."},
    {NULL,                      NULL,                                   0,                              NULL}
};

static struct PyModuleDef _module = {
    PyModuleDef_HEAD_INIT,
    "_aointernal",
    "Basic interface of libao for Python",
    -1,
    _methods
};

PyMODINIT_FUNC PyInit__aointernal() {
    PyObject *mod;
    mod = PyModule_Create(&_module);
    PyModule_AddIntMacro(mod, AO_FMT_LITTLE);
    PyModule_AddIntMacro(mod, AO_FMT_BIG);
    PyModule_AddIntMacro(mod, AO_FMT_NATIVE);
    return mod;
}
