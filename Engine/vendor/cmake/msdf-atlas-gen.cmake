set(MSDF_ATLAS_BUILD_STANDALONE OFF CACHE BOOL "")

FETCHCONTENT_DECLARE(
	msdf-atlas-gen
	GIT_REPOSITORY https://github.com/Chlumsky/msdf-atlas-gen.git
	GIT_TAG master
)

FETCHCONTENT_MAKEAVAILABLE(msdf-atlas-gen)