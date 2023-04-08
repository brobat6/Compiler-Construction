**General Template:**

**Rule x:**

```rule```

- Type : 
- syn_next : 
- inh_next : 
- token_info : 
- child_1 : 
- child_2 : 
- child_3 : 
- child_4 : 
- temp : 

**Rule 1:**

```<program> -> <moduleDeclarations> <otherModules_1> <driverModule> <otherModules_2>```

- Type : 1
- syn_next : 
- inh_next : 
- token_info : 
- child_1 : moduleDeclarations
- child_2 : otherModules_1
- child_3 : driverModule
- child_4 : otherModules_2
- temp : 

**Rule 2:**

```<moduleDeclarations_1> -> <moduleDeclaration><moduleDeclarations_2>```

- Type : 2
- child_1 : moduleDeclaration
- syn_next : moduleDeclarations_2 // Alternatively, make it child_2. To make code simpler. Consider changing or not, later on.

**Rule 3:**

```<moduleDeclarations> -> @```

- NULL (This way, in rule 2, the last moduleDeclarations will have syn_next == NULL, and we will know to stop our traversal)

**Rule 4:**

```<moduleDeclaration> -> DECLARE MODULE ID SEMICOL```

- Type : 3
- token_info : ID.token

**Rule 5:**

```<otherModules_1> -> <module> <otherModules_2>```

- Type : 4
- child_1 : module
- syn_next : otherModules_2

**Rule 6:**

- NULL



