#pragma once

#include "core/EventDelegate.h"
#include "GraphContext.h"
#include "GraphEventHandler.h"
#include "gui/ImGuiExtended.h"
#include <memory>
#include <type_traits>
#include <vector>

namespace BHive
{
	typedef UUID NodeID;
	typedef UUID PinID;
	typedef UUID LinkID;

	struct Pin;
	struct Node;
	struct Link;

	DECLARE_EVENT(OnGraphContextMenu, const ImVec2 &);
	DECLARE_EVENT(OnNodeContextMenu, Node *);
	DECLARE_EVENT(OnGraphDragDrop, const ImVec2 &, const ImGuiPayload *)

	struct FGridColor
	{
		ImU32 mBackground = IM_COL32(33, 41, 45, 255);
		ImU32 mGrid = IM_COL32(200, 200, 200, 40);
		ImU32 mSubGrid = IM_COL32(100, 100, 100, 80);
	};

	struct FGridStyle
	{
		float mGridSize = 50.0f;
		float mGridDivisions = 10.f;
		FGridColor mColor;
	};

	struct Graph
	{
	private:
		static inline int sNumInstances = 0;

	public:
		Graph();
		Graph(const std::string &name);

		void update();

		void addNode(const ImVec2 &pos, const std::shared_ptr<Node> &node);

		template <typename T, typename... Args>
		void addNode(const ImVec2 &pos, Args &&...args)
			requires(std::is_base_of_v<Node, T>)
		{
			addNode(pos, std::make_shared<T>(std::forward<Args>(args)...));
		}

		const ImVec2 &getPos() { return mContext.mOrigin; }

		const ImVec2 &getScroll() { return mContext.mScroll; }

		ImVec2 grid2Screen(const ImVec2 &pos);

		ImVec2 screen2Grid(const ImVec2 &pos);

		bool isAnyNodeSelected();

		bool isAnyNodeHovered();

		void select(const NodeID &id);

		void deselect(const NodeID &id);

		void clearSelection();

		bool getMouseClickState() const { return mIsMouseClicked; }

		void consumeMouseClickState();

		void setGraphContext(const OnGraphContextMenuEvent &event);

		void setGraphDragDropEvent(const OnGraphDragDropEvent &event);

		void setIsDraggingNode(bool dragging);

		bool isDraggingNode() const { return mIsDraggingNode; }

		FContextConfig &getConfig() { return mContext.mConfig; }

	private:
		void drawGrid(const FGridStyle &style, const FGraphContext &context);

	private:
		std::string mName = "";
		FGridStyle mStyle;
		FGraphContext mContext;
		std::unordered_map<NodeID, std::shared_ptr<Node>> mNodes;
		std::vector<std::weak_ptr<Node>> mSelectedNodes;
		bool mIsMouseClicked{false};

		float mScaleTarget{1.f};
		OnGraphContextMenuEvent mGraphContextMenu;
		OnGraphDragDropEvent mGraphDragDropPayload;

		MouseDragHandler mDragHandler;
		bool mIsDraggingNode{false};
	};

	struct FNodeStyle
	{
		ImVec4 mHeaderTitleColor{1, 1, 1, 1};
		ImVec4 mHeaderColor{.8f, 0, 0, 1};
		ImVec4 mBackgroundColor{.3f, .3f, .3f, .8f};
		float mRounding = 1.0f;
		float mBorderThickness = 2.0f;
		ImVec4 mBorderColor{.6f, .6f, .6f, 1};
		ImVec4 mBorderHoveredColor{.7f, .6f, .4f, 1};
		ImVec4 mBorderSelectedColor{.7f, .5f, 0, 1};
		ImVec4 mPadding{1, 1, 1, 1};
	};

	struct Node
	{
		Node() = default;
		virtual ~Node() = default;

		void update();

		virtual void draw() {}

		Node *setGraph(Graph *graph);

		Node *setTitle(const std::string &title);

		Node *setUID(const NodeID &id);

		Node *setPos(const ImVec2 &pos);

		Node *setSelected(bool selected);

		inline const NodeID &getUID() const { return mUID; }

		inline const std::string &getTitle() const { return mTitle; }

		inline const ImVec2 &getPos() { return mPos; }

		inline bool isHovered() const { return mIsHovered; }

		inline bool isSelected() const { return mIsSelected; }

		bool isHovering() const;

		void nodeContext(const OnNodeContextMenuEvent &context);

	private:
		std::string mTitle{};
		FNodeStyle mStyle{};
		NodeID mUID;
		Graph *mGraph = nullptr;
		ImVec2 mPos{};
		ImVec2 mSize{};
		bool mIsHovered{false}, mIsSelected{false};
		bool mIsDragging{false};
		OnNodeContextMenuEvent mOnNodeContextMenu;

		friend struct Graph;
	};
} // namespace BHive