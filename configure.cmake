# Build architecture. (x86 | x64)
set(TARGET_ARCH "x86" CACHE INTERNAL "Which arch we want to build for.")

# If nothing specified, then we will obtain environment variable: METAMOD_SOURCE
set(METAMOD_PATH "" CACHE INTERNAL "Path to metamod")

# If nothing specified, then we will obtain environment variable: SOURCEMOD
set(SOURCEMOD_PATH "" CACHE INTERNAL "Path to sourcemod")