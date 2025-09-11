// #include "core/application.h"
// #include "core/time.h"
// #include "gfx/rendercommand.h"
// #include "gfx/shaderinstance.h"
// #include "gfx/shadermanager.h"
// #include "gfx/textures/rendertargetcube.h"
// #include "gfx/uniformbuffer.h"
// #include "reflectionlayer.h"
// #include "renderers/renderer.h"
//
// namespace bhive
//{
//	struct reflectionmaterial
//	{
//		glm::vec4 color;
//		alignas(16) float ior;
//		float reflective;
//	};
//
//	void reflectionlayer::onattach()
//	{
//		mreflectionshader = shadermanager::get().load(resource_path "shaders/reflection.glsl");
//		auto &data = mreflectionshader->getrelectiondata();
//		auto ubo = data.uniformbuffers.at("material");
//		minstance = createref<shaderinstance>(mreflectionshader);
//
//		mreflectubo = createref<uniformbuffer>(ubo.binding, ubo.size);
//
//		mplane = createref<pplane>(100.f, 100.f);
//		msphere = createref<psphere>(2.f);
//
//		mrelfectiontarget = createref<rendertargetcube>(256, eformat::rgba8);
//
//		mplaneindirect = createref<indirectrenderable>();
//		mplaneindirect->init(mplane);
//
//		msphereindirect = createref<indirectrenderable>();
//		msphereindirect->init(msphere);
//
//		auto size = application::get().getwindow().getsize();
//		mcamera = editorcamera(75.f, size.x / (float)size.y, .01f, 1000.f);
//
//		rendercommand::clearcolor(.3f, .3f, .3f);
//	}
//
//	void reflectionlayer::ondetach()
//	{
//	}
//
//	void reflectionlayer::onupdate(float dt)
//	{
//		mcamera.processinput();
//
//		rendercommand::clear();
//
//		mreflectionshader->bind();
//
//		renderer::begin();
//
//		 capture reflections
//		{
//			for (int i = 0; i < 6; i++)
//			{
//				mrelfectiontarget->bind(i);
//
//				drawscene();
//
//				mrelfectiontarget->unbind();
//			}
//		}
//
//		auto size = application::get().getwindow().getsize();
//		rendercommand::setviewport(0, 0, size.x, size.y);
//
//		renderer::submitcamera(mcamera.getprojection(), mcamera.getview());
//
//		mrelfectiontarget->gettargettexture()->bind();
//
//		drawscene();
//
//		renderer::end();
//	}
//	void reflectionlayer::onevent(event &e)
//	{
//		mcamera.onevent(e);
//
//		eventdispatcher dispatcher(e);
//		dispatcher.dispatch(this, &reflectionlayer::onwindowresize);
//		dispatcher.dispatch(this, &reflectionlayer::onkeyevent);
//	}
//
//	bool reflectionlayer::onwindowresize(windowresizeevent &e)
//	{
//		rendercommand::setviewport(0, 0, e.x, e.y);
//		mcamera.resize(e.x, e.y);
//		return false;
//	}
//	bool reflectionlayer::onkeyevent(keyevent &e)
//	{
//		if (e.key == key::r)
//		{
//			mreflectionshader->recompile();
//
//			return true;
//		}
//		return false;
//	}
//	void reflectionlayer::drawscene()
//	{
//		static float radius = 9.0f;
//		float x = glm::cos(time::get()) * radius;
//		float y = glm::sin(time::get()) * radius;
//
//		reflectionmaterial material{};
//
//		material.color = {1, 1, 1, 1};
//		material.ior = 1.33f;
//		material.reflective = 0.5f;
//		mreflectubo->setdata(&material, sizeof(reflectionmaterial));
//
//		mplaneindirect->draw(ftransform({0, -4, 0}, {0, 0, 0}));
//
//		material.color = {1, 0, 1, 1};
//		material.ior = 1.52f;
//		material.reflective = 0.5f;
//		mreflectubo->setdata(&material, sizeof(reflectionmaterial));
//
//		msphereindirect->draw(ftransform({x, 0, y}));
//
//		material.color = {1, .5f, 0, .5f};
//		material.ior = 2.42f;
//		material.reflective = 0.3f;
//		mreflectubo->setdata(&material, sizeof(reflectionmaterial));
//
//		msphereindirect->draw(ftransform({-5, 0, 0}));
//	}
// } // namespace bhive