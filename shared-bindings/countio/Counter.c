
#include <stdint.h>

#include "lib/utils/context_manager_helpers.h"
#include "py/objproperty.h"
#include "py/runtime.h"
#include "py/runtime0.h"
#include "shared-bindings/microcontroller/Pin.h"
#include "shared-bindings/countio/Counter.h"
#include "shared-bindings/util.h"

//| .. currentmodule:: countio
//|
//| :class:`Counter` -- Track the count of edge transistions (pulses) on a given pin
//| ====================================================================================
//|
//| Counter will keep track of the number of edge transistions (pulses) on a given pin
//|
//| .. class:: Counter(pin_a)
//|
//|   Create an Counter object associated with the given pin. It tracks the number of 
//|   pulses relative when the object is contructed.
//|
//|   :param ~microcontroller.Pin pin_a: Pin to read pulses from.
//|   
//|
//|   For example::
//|
//|     import countio
//|     import time
//|     from board import *
//|
//|     pin_counter = countio.Counter(board.D1)
//|     
//|     while True:
//|         count = pin_counter.count
//|         if count == None 10:
//|             pin_counter.reset()
//|         print(pin_counter.count)
//|
STATIC mp_obj_t countio_counter_make_new(const mp_obj_type_t *type, size_t n_args, const mp_obj_t *pos_args, mp_map_t *kw_args) {
    enum { ARG_pin_a };
    static const mp_arg_t allowed_args[] = {
        { MP_QSTR_pin_a, MP_ARG_REQUIRED | MP_ARG_OBJ }
        
    };
    mp_arg_val_t args[MP_ARRAY_SIZE(allowed_args)];
    mp_arg_parse_all(n_args, pos_args, kw_args, MP_ARRAY_SIZE(allowed_args), allowed_args, args);

    const mcu_pin_obj_t* pin_a = validate_obj_is_free_pin(args[ARG_pin_a].u_obj);
    

    countio_counter_obj_t *self = m_new_obj(countio_counter_obj_t);
    self->base.type = &countio_counter_type;

    common_hal_countio_counter_construct(self, pin_a);

    return MP_OBJ_FROM_PTR(self);
}

//|   .. method:: deinit()
//|
//|      Deinitializes the Counter and releases any hardware resources for reuse.
//|
STATIC mp_obj_t countio_counter_deinit(mp_obj_t self_in) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    common_hal_countio_counter_deinit(self);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_1(countio_counter_deinit_obj, countio_counter_deinit);

STATIC void check_for_deinit(countio_counter_obj_t *self) {
    if (common_hal_countio_counter_deinited(self)) {
        raise_deinited_error();
    }
}

//|   .. method:: __enter__()
//|
//|      No-op used by Context Managers.
//|
//  Provided by context manager helper.

//|   .. method:: __exit__()
//|
//|      Automatically deinitializes the hardware when exiting a context. See
//|      :ref:`lifetime-and-contextmanagers` for more info.
//|
STATIC mp_obj_t countio_counter_obj___exit__(size_t n_args, const mp_obj_t *args) {
    (void)n_args;
    common_hal_countio_counter_deinit(args[0]);
    return mp_const_none;
}
STATIC MP_DEFINE_CONST_FUN_OBJ_VAR_BETWEEN(countio_counter___exit___obj, 4, 4, countio_counter_obj___exit__);


//|   .. attribute:: count
//|
//|     The current count in terms of pulses.
//|
//|
STATIC mp_obj_t countio_counter_obj_get_count(mp_obj_t self_in) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    return mp_obj_new_int(common_hal_countio_counter_get_count(self));
}
MP_DEFINE_CONST_FUN_OBJ_1(countio_counter_get_count_obj, countio_counter_obj_get_count);

STATIC mp_obj_t countio_counter_obj_set_count(mp_obj_t self_in, mp_obj_t new_count) {
    countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
    check_for_deinit(self);

    common_hal_countio_counter_set_count(self, mp_obj_get_int(new_count));
    return mp_const_none;
}
MP_DEFINE_CONST_FUN_OBJ_2(countio_counter_set_count_obj, countio_counter_obj_set_count);

const mp_obj_property_t countio_counter_count_obj = {
    .base.type = &mp_type_property,
    .proxy = {(mp_obj_t)&countio_counter_get_count_obj,
              (mp_obj_t)&countio_counter_set_count_obj,
              (mp_obj_t)&mp_const_none_obj},
};

STATIC mp_obj_t countio_counter_reset(mp_obj_t self_in){
	countio_counter_obj_t *self = MP_OBJ_TO_PTR(self_in);
	check_for_deinit(self);
	//set the position to zero for reset
	common_hal_countio_counter_reset(self);
	return mp_const_none;
}
	

MP_DEFINE_CONST_FUN_OBJ_1(countio_counter_reset_obj, countio_counter_reset);

STATIC const mp_rom_map_elem_t countio_counter_locals_dict_table[] = {
    // Methods
    { MP_ROM_QSTR(MP_QSTR_deinit), MP_ROM_PTR(&countio_counter_deinit_obj) },
    { MP_ROM_QSTR(MP_QSTR___enter__), MP_ROM_PTR(&default___enter___obj) },
    { MP_ROM_QSTR(MP_QSTR___exit__), MP_ROM_PTR(&countio_counter___exit___obj) },
    { MP_ROM_QSTR(MP_QSTR_count), MP_ROM_PTR(&countio_counter_count_obj) },
    { MP_ROM_QSTR(MP_QSTR_reset), MP_ROM_PTR(&countio_counter_reset_obj) },
};
STATIC MP_DEFINE_CONST_DICT(countio_counter_locals_dict, countio_counter_locals_dict_table);

const mp_obj_type_t countio_counter_type = {
    { &mp_type_type },
    .name = MP_QSTR_Counter,
    .make_new = countio_counter_make_new,
    .locals_dict = (mp_obj_dict_t*)&countio_counter_locals_dict,
};
