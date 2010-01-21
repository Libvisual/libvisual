#include <libvisual/libvisual.h>

#include "avs.h"

typedef struct {
    AvsRunnableContext      *ctx;
    AvsRunnableVariableManager  *vm;
    AvsRunnable         *runnable;
    AvsNumber           foo, bar, baz;
} Private;

int load_runnable( Private *priv, char *buf )
{
    AvsRunnable *obj = avs_runnable_new(priv->ctx);
    avs_runnable_set_variable_manager(obj, priv->vm);
    priv->runnable = obj;
    avs_runnable_compile(obj, (unsigned char *)buf, strlen(buf));
    return 0;
}

int run(Private *priv)
{
    avs_runnable_execute(priv->runnable);
    return 0;
}

int main(int argc, char **argv)
{
    char *script = "foo=3*0.2;";
    Private *priv;
    priv = visual_mem_new0 (Private, 1);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();

    avs_runnable_variable_bind(priv->vm, "foo", &priv->foo);
    avs_runnable_variable_bind(priv->vm, "bar", &priv->bar);
    avs_runnable_variable_bind(priv->vm, "baz", &priv->baz);
  
    load_runnable(priv, script);

    run(priv);

    printf("foo: %f, bar: %f, baz: %f\n", priv->foo, priv->bar, priv->baz);
    return 0;
}
