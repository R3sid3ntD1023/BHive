#include "Graph.h"

namespace BHive
{
	Graph::Graph()
		: Graph("Graph" + std::to_string(sNumInstances))
	{
		mContext.mConfig.mColor = mStyle.mColor.mBackground;
	}

	Graph::Graph(const std::string &name)
		: mName(name)
	{
		sNumInstances++;
	}

	void Graph::update()
	{
		mIsMouseClicked = ImGui::IsMouseClicked(ImGuiMouseButton_Left);

		mContext.begin();
		ImGui::GetIO().IniFilename = nullptr;

		if (ImGui::BeginPopupContextWindow())
		{
			if (mGraphContextMenu)
				mGraphContextMenu(ImGui::GetMousePos() - getPos() + getScroll());

			ImGui::EndPopup();
		}

		drawGrid(mStyle, mContext);

		for (auto &[id, node] : mNodes)
		{
			node->update();
		}

		mContext.end();

		bool hovered = ImGui::IsWindowHovered(ImGuiHoveredFlags_ChildWindows);

		if (hovered)
		{
			if (!isAnyNodeHovered())
			{
				if (ImGui::IsMouseClicked(ImGuiMouseButton_Left))
				{
					clearSelection();
				}
			}

			if (ImGui::BeginDragDropTarget())
			{
				auto payload = ImGui::AcceptDragDropPayload(mContext.mConfig.mDragDropName.c_str());
				if (payload && mGraphDragDropPayload)
				{
					mGraphDragDropPayload(screen2Grid(ImGui::GetMousePos()), payload);
				}
				ImGui::EndDragDropTarget();
			}
		}
	}

	void Graph::addNode(const ImVec2 &pos, const std::shared_ptr<Node> &node)
	{
		node->setGraph(this)->setPos(pos);
		mNodes.emplace(node->getUID(), node);
	}

	ImVec2 Graph::grid2Screen(const ImVec2 &pos)
	{
		if (ImGui::GetCurrentContext() == mContext.GetRawContext())
			return pos + mContext.mScroll;

		return (pos + mContext.mScroll) * mContext.mScale + mContext.mOrigin;
	}

	ImVec2 Graph::screen2Grid(const ImVec2 &pos)
	{
		if (ImGui::GetCurrentContext() == mContext.GetRawContext())
			return pos - mContext.mScroll;

		return (pos - mContext.mOrigin) / mContext.mScale - mContext.mScroll;
	}

	bool Graph::isAnyNodeSelected()
	{
		return mSelectedNodes.size() > 0;
	}

	bool Graph::isAnyNodeHovered()
	{
		return std::any_of(mNodes.begin(), mNodes.end(), [](const auto &node) { return node.second->isHovered(); });
	}

	void Graph::select(const NodeID &id, bool append)
	{
		if (mNodes.contains(id))
		{
			if (!append)
			{
				clearSelection();
			}

			auto node = mNodes.at(id);
			node->setSelected(true);
			mSelectedNodes.push_back(node);
		}
	}

	void Graph::clearSelection()
	{
		for (auto &selected : mSelectedNodes)
		{
			selected.lock()->setSelected(false);
		}
		mSelectedNodes.clear();
	}

	void Graph::consumeMouseClickState()
	{
		mIsMouseClicked = true;
	}

	void Graph::setGraphContext(const OnGraphContextMenuEvent &event)
	{
		mGraphContextMenu = event;
	}

	void Graph::setGraphDragDropEvent(const OnGraphDragDropEvent &event)
	{
		mGraphDragDropPayload = event;
	}

	void Graph::drawGrid(const FGridStyle &style, const FGraphContext &context)
	{
		auto draw_list = ImGui::GetWindowDrawList();
		auto grid_size = ImGui::GetWindowSize();

		float sub_grid_steps = style.mGridSize / style.mGridDivisions;

		for (float x = fmodf(context.mScroll.x, style.mGridSize); x < grid_size.x; x += style.mGridSize)
		{
			draw_list->AddLine(ImVec2{x, 0.0f}, ImVec2{x, grid_size.y}, style.mColor.mGrid, 2.f);
		}

		for (float y = fmodf(context.mScroll.y, style.mGridSize); y < grid_size.y; y += style.mGridSize)
		{
			draw_list->AddLine(ImVec2{0.0f, y}, ImVec2{grid_size.x, y}, style.mColor.mGrid, 2.f);
		}

		if (mContext.mScale > .7f)
		{
			for (float x = fmodf(context.mScroll.x, sub_grid_steps); x < grid_size.x; x += sub_grid_steps)
			{
				draw_list->AddLine(ImVec2{x, 0.0f}, ImVec2{x, grid_size.y}, style.mColor.mSubGrid, 1.f);
			}

			for (float y = fmodf(context.mScroll.y, sub_grid_steps); y < grid_size.y; y += sub_grid_steps)
			{
				draw_list->AddLine(ImVec2{0.0f, y}, ImVec2{grid_size.x, y}, style.mColor.mSubGrid, 1.f);
			}
		}
	}

	void Node::update()
	{
		bool mouse_state = mGraph->getMouseClickState();

		auto draw_list = ImGui::GetWindowDrawList();
		ImDrawListSplitter splitter;
		splitter.Split(draw_list, 2);

		// foreground
		splitter.SetCurrentChannel(draw_list, 1);

		ImGui::PushID(this);

		auto offset = mGraph->grid2Screen({0.0f, 0.0f});
		ImVec2 paddingTL = {mStyle.mPadding.x, mStyle.mPadding.y};
		ImVec2 paddingBR = {mStyle.mPadding.z, mStyle.mPadding.w};

		ImGui::SetCursorScreenPos(mPos + offset);

		ImGui::BeginGroup();

		// Header
		ImGui::BeginGroup();
		ImGui::TextColored(mStyle.mHeaderTitleColor, "%s", mTitle.c_str());
		ImGui::Spacing();
		ImGui::EndGroup();
		float headerH = ImGui::GetItemRectSize().y;
		float titleW = ImGui::GetItemRectSize().x;

		// draw contents
		ImGui::BeginGroup();
		draw();
		ImGui::Dummy({0, 0});
		ImGui::EndGroup();

		ImGui::EndGroup();

		auto rect = ImGui::GetItemRect();
		mSize = rect.GetSize();
		auto header_size = ImVec2(mSize.x, headerH);

		auto header_color = ImGui::GetColorU32(mStyle.mHeaderColor);
		auto node_color = ImGui::GetColorU32(mStyle.mBackgroundColor);
		auto border_color = ImGui::GetColorU32(mStyle.mBorderColor);
		auto border_hovered_color = ImGui::GetColorU32(mStyle.mBorderHoveredColor);
		auto border_selected_color = ImGui::GetColorU32(mStyle.mBorderSelectedColor);
		auto current_border_color = mIsSelected ? border_selected_color : (mIsHovered ? border_hovered_color : border_color);

		float rounding = mStyle.mRounding;
		float thickness = mStyle.mBorderThickness;
		auto ptl = paddingTL;
		auto pbr = paddingBR;

		// background
		splitter.SetCurrentChannel(draw_list, 0);

		draw_list->AddRectFilled(rect.Min - ptl, rect.Max + pbr, node_color, rounding, ImDrawFlags_RoundCornersAll);
		draw_list->AddRectFilled(rect.Min - ptl, rect.Min + header_size + pbr, header_color, rounding, ImDrawFlags_RoundCornersTop);
		draw_list->AddRect(rect.Min - ptl, rect.Max + pbr, current_border_color, rounding, ImDrawFlags_RoundCornersAll, thickness);

		splitter.Merge(draw_list);
		splitter.ClearFreeMemory();

		mIsHovered = isHovering() && !ImGui::IsAnyItemHovered();
		if (mIsHovered)
		{
			if (mouse_state)
			{
				mGraph->consumeMouseClickState();

				bool append_seletion = ImGui::IsKeyDown(ImGuiKey_ModCtrl);
				mGraph->select(getUID(), append_seletion);
				mIsDragging = true;
			}
		}

		if (mIsDragging)
		{
			mPos += ImGui::GetIO().MouseDelta;

			if (ImGui::IsMouseReleased(ImGuiMouseButton_Left))
			{
				mIsDragging = false;
			}
		}

		if (ImGui::BeginPopupContextItem("NodeContextMenu", ImGuiPopupFlags_MouseButtonRight))
		{
			if (mOnNodeContextMenu)
				mOnNodeContextMenu(this);

			ImGui::EndPopup();
		}

		ImGui::PopID();
	}

	Node *Node::setGraph(Graph *graph)
	{
		mGraph = graph;
		return this;
	}

	//------------------NODE----------------------------------------------
	Node *Node::setTitle(const std::string &title)
	{
		mTitle = title;
		return this;
	}

	Node *Node::setUID(const NodeID &id)
	{
		mUID = id;
		return this;
	}

	Node *Node::setPos(const ImVec2 &pos)
	{
		mPos = pos;
		return this;
	}

	Node *Node::setSelected(bool selected)
	{
		mIsSelected = selected;
		return this;
	}

	bool Node::isHovering() const
	{
		ImVec2 paddingTL = {mStyle.mPadding.x, mStyle.mPadding.y};
		ImVec2 paddingBR = {mStyle.mPadding.z, mStyle.mPadding.w};
		return ImGui::IsMouseHoveringRect(mGraph->grid2Screen(mPos - paddingTL), mGraph->grid2Screen(mPos + mSize - paddingBR));
	}

	void Node::nodeContext(const OnNodeContextMenuEvent &context)
	{
		mOnNodeContextMenu = context;
	}

} // namespace BHive