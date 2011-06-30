#include <libvisual/libvisual.h>
#include <stdlib.h>
#include <stdio.h>
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

extern AvsNumber _rand(AvsNumber val);

int main(int argc, char **argv)
{
    char *script = "foo=3*0.2; bar=foo*10; baz=foo*bar;";
    Private *priv;
    AvsRunnableVariable *var = NULL;
    AvsILTreeNode *node, *tmp_node;
    AvsRunnable *run1, *run2, *run3;
    priv = visual_mem_new0 (Private, 1);

    priv->ctx = avs_runnable_context_new();
    priv->vm = avs_runnable_variable_manager_new();
    avs_runnable_variable_bind(priv->vm, "foo", &priv->foo);
    avs_runnable_variable_bind(priv->vm, "bar", &priv->bar);
    avs_runnable_variable_bind(priv->vm, "baz", &priv->baz);
    load_runnable(priv, script);

    run(priv);

    printf("foo: %f, bar: %f, baz: %f\n", priv->foo, priv->bar, priv->baz);

/*
    for(node = tmp_node = avs_il_tree_base(&priv->ctx->assembler.tree); tmp_node; node = tmp_node)
    {
        tmp_node = node->next;
        visual_mem_free(node);
    }
  */   
    visual_mem_free(priv);

    return 0;
}
