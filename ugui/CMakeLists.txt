# Finds all source files in the current directory
# and save the name to the DIR_Fonts_SRCS)variable
aux_source_directory(. DIR_ugui_SRCS)

# Generate the link library
add_library(ugui ${DIR_ugui_SRCS})