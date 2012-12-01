class LunaWrapper {
  public:
    LunaWrapper(lua_State *L) {
      printf("in constructor\n");
    }

    int foo(lua_State *L) {
      printf("in foo\n");
    }

    ~LunaWrapper() {
      printf("in destructor\n");
    }

    static const char className[];
    static const Luna<LunaWrapper>::RegType Register[];
};

const char LunaWrapper::className[] = "LunaWrapper";
const Luna<LunaWrapper>::RegType LunaWrapper::Register[] = {
  { "foo", &LunaWrapper::foo },
  { 0 }
};

