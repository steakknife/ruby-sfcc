
#include "cim_instance.h"
#include "cim_object_path.h"

static void
dealloc(CIMCInstance *inst)
{
/*  fprintf(stderr, "Sfcc_dealloc_cim_instance %p\n", inst); */
  inst->ft->release(inst);
}

/**
 * call-seq:
 *   property(name)
 *
 * Gets a named property value, where name is a Symbol or String
 */
static VALUE property(VALUE self, VALUE name)
{
  CIMCInstance *ptr;
  CIMCStatus status;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);
  data = ptr->ft->getProperty(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);
  sfcc_rb_raise_if_error(status, "Can't retrieve property '%s'", to_charptr(name));
  return Qnil;
}

/**
 * call-seq:
 *   instance.each_property do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the property name and
 * its value
 *
 */
static VALUE each_property(VALUE self)
{
  CIMCInstance *ptr;
  CIMCStatus status;
  int k=0;
  int num_props=0;
  CIMCString *property_name = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);

  num_props = ptr->ft->getPropertyCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyAt(ptr, k, &property_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_name ? rb_str_intern(rb_str_new2(property_name->ft->getCharPtr(property_name, NULL))) : Qnil), sfcc_cimdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve property #%d", k);
      } 
      if (property_name) CMRelease(property_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve property count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   property_count()
 *
 * Gets the number of properties contained in this Instance
 */
static VALUE property_count(VALUE self)
{
  CIMCInstance *ptr;
  Data_Get_Struct(self, CIMCInstance, ptr);
  return UINT2NUM(ptr->ft->getPropertyCount(ptr, NULL));
}

/**
 * call-seq:
 *   set_property(name, value)
 *
 * Adds/replaces a names property
 */
static VALUE set_property(VALUE self, VALUE name, VALUE value)
{
  CIMCInstance *ptr;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);
  data = sfcc_value_to_cimdata(value);
  ptr->ft->setProperty(ptr, to_charptr(name), &data.value, data.type);

  return value;
}

/**
 * call-seq:
 *  object_path()
 *
 * Generates an ObjectPath out of the nameSpace, classname and
 * key propeties of this Instance.
 */
static VALUE object_path(VALUE self)
{
  CIMCInstance *ptr;
  CIMCObjectPath *op;
  Data_Get_Struct(self, CIMCInstance, ptr);
  op = ptr->ft->getObjectPath(ptr, NULL);
  return Sfcc_wrap_cim_object_path(op);
}

/**
 * call-seq:
 *   set_property_filter(property_list, keys)
 *
 * Directs CIMC to ignore any setProperty operations for this
 * instance for any properties not in this list.
 *
 * +property_list+ If not nil, the members of the array define one
 * or more Property names to be accepted by set_property operations.
 *
 * +keys+ Array of key property names of this instance. This array
 * must be specified.
 *
 */
static VALUE set_property_filter(VALUE self, VALUE property_list, VALUE keys)
{
  CIMCStatus status;
  CIMCInstance *ptr;
  char **prop_a;
  char **key_a;

  Data_Get_Struct(self, CIMCInstance, ptr);

  prop_a = sfcc_value_array_to_string_array(property_list);
  key_a = sfcc_value_array_to_string_array(keys);

  status = ptr->ft->setPropertyFilter(ptr, prop_a, key_a);
  free(prop_a);
  free(key_a);

  sfcc_rb_raise_if_error(status, "Can't set property filter");
  return self;
}

/**
 * call-seq:
 *   qualifier(name)
 *
 * gets a named qualifier value
 */
static VALUE qualifier(VALUE self, VALUE name)
{
  CIMCInstance *ptr;
  CIMCStatus status;
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCInstance, ptr);
  data = ptr->ft->getQualifier(ptr, to_charptr(name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve qualifier '%s'", to_charptr(name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_qualifier do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the qualifier name and
 * its value
 *
 */
static VALUE each_qualifier(VALUE self)
{
  CIMCInstance *ptr;
  CIMCStatus status;
  int k=0;
  int num_props=0;
  CIMCString *qualifier_name = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);

  num_props = ptr->ft->getQualifierCount(ptr, &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getQualifierAt(ptr, k, &qualifier_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (qualifier_name ? rb_str_new2(qualifier_name->ft->getCharPtr(qualifier_name, NULL)) : Qnil), sfcc_cimdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve qualifier #%d", k);
      } 
      if (qualifier_name) CMRelease(qualifier_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve qualifier count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   qualifier_count()
 *
 * Gets the number of qualifiers in this instance
 */
static VALUE qualifier_count(VALUE self)
{
  CIMCInstance *ptr;
  Data_Get_Struct(self, CIMCInstance, ptr);
  return UINT2NUM(ptr->ft->getQualifierCount(ptr, NULL));
}

/**
 * call-seq:
 *   property_qualifier(property_name, qualifier_name)
 *
 * gets a named property qualifier value
 */
static VALUE property_qualifier(VALUE self, VALUE property_name, VALUE qualifier_name)
{
  CIMCInstance *ptr;
  CIMCStatus status;
  CIMCData data;
  memset(&status, 0, sizeof(CIMCStatus));
  Data_Get_Struct(self, CIMCInstance, ptr);
  data = ptr->ft->getPropertyQualifier(ptr, to_charptr(property_name),
                                        to_charptr(qualifier_name), &status);
  if ( !status.rc )
    return sfcc_cimdata_to_value(data);

  sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier '%s'", to_charptr(qualifier_name));
  return Qnil;
}

/**
 * call-seq:
 *   cimclass.each_property_qualifier(property_name) do |name, value|
 *      ...
 *   end
 *
 * enumerates properties yielding the property qualifier name and
 * its value
 *
 */
static VALUE each_property_qualifier(VALUE self, VALUE property_name)
{
  CIMCInstance *ptr;
  CIMCStatus status;
  int k=0;
  int num_props=0;
  CIMCString *property_qualifier_name = NULL;
  CIMCData data;
  Data_Get_Struct(self, CIMCInstance, ptr);

  num_props = ptr->ft->getPropertyQualifierCount(ptr, to_charptr(property_name), &status);
  if (!status.rc) {
    for (; k < num_props; ++k) {
      data = ptr->ft->getPropertyQualifierAt(ptr, to_charptr(property_name), k, &property_qualifier_name, &status);
      if (!status.rc) {
        rb_yield_values(2, (property_qualifier_name ? rb_str_new2(property_qualifier_name->ft->getCharPtr(property_qualifier_name, NULL)) : Qnil), sfcc_cimdata_to_value(data));
      }
      else {
        sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier #%d", k);
      } 
      if (property_qualifier_name) CMRelease(property_qualifier_name);
    }
  }
  else {
    sfcc_rb_raise_if_error(status, "Can't retrieve property qualifier count");
  }
  return Qnil;
}

/**
 * call-seq:
 *   property_qualifier_count(property_name)
 *
 * Gets the number of qualifiers contained in this property
 */
static VALUE property_qualifier_count(VALUE self, VALUE property_name)
{
  CIMCInstance *ptr;
  Data_Get_Struct(self, CIMCInstance, ptr);
  return UINT2NUM(ptr->ft->getPropertyQualifierCount(ptr, to_charptr(property_name), NULL));
}

/**
 * call-seq
 *   new()
 *
 * Creates an instance from in +object_path+
 *
 */
static VALUE new(VALUE klass, VALUE object_path)
{
  CIMCStatus status;
  CIMCInstance *ptr;
  CIMCObjectPath *op;

  Data_Get_Struct(object_path, CIMCObjectPath, op);
  ptr = cimcEnv->ft->newInstance(cimcEnv, op, &status);

  if (!status.rc)
    return Sfcc_wrap_cim_instance(ptr);
  sfcc_rb_raise_if_error(status, "Can't create instance");
  return Qnil;
}

VALUE
Sfcc_wrap_cim_instance(CIMCInstance *instance)
{
  return Data_Wrap_Struct(cSfccCimInstance, NULL, dealloc, instance);
}

VALUE cSfccCimInstance;
void init_cim_instance()
{
  VALUE sfcc = rb_define_module("Sfcc");
  VALUE cimc = rb_define_module_under(sfcc, "Cim");

  /**
   * an instance of a CIM class
   */
  VALUE klass = rb_define_class_under(cimc, "Instance", rb_cObject);
  cSfccCimInstance = klass;

  rb_define_singleton_method(klass, "new", new, 1);
  rb_define_method(klass, "property", property, 1);
  rb_define_method(klass, "each_property", each_property, 0);
  rb_define_method(klass, "property_count", property_count, 0);
  rb_define_method(klass, "set_property", set_property, 2);
  rb_define_method(klass, "object_path", object_path, 0);
  rb_define_method(klass, "set_property_filter", set_property_filter, 3);
  rb_define_method(klass, "qualifier", qualifier, 1);
  rb_define_method(klass, "each_qualifier", each_qualifier, 0);
  rb_define_method(klass, "qualifier_count", qualifier_count, 0);
  rb_define_method(klass, "property_qualifier", property_qualifier, 2);
  rb_define_method(klass, "each_property_qualifier", each_property_qualifier, 1);
  rb_define_method(klass, "property_qualifier_count", property_qualifier_count, 1);
}
