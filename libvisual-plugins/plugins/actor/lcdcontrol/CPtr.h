#ifndef __CPTR_
#define __CPTR_

template <class T>
T getObjectFromCPtr( JNIEnv *env, jobject cptr )
{
    T obj;
    jclass classPtr = env->GetObjectClass( cptr );
    jfieldID CPtr_peer_ID = env->GetFieldID( classPtr, "peer", "J" );
    jlong *peer = (jlong *) env->GetLongField( cptr, CPtr_peer_ID );

    obj = ( T ) peer;

    return obj;
}

#endif
