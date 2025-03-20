#pragma once

namespace Modex
{
	// Flame Graph Rules as I find them;
	// 1. Each function is a node.
	// 2. Each node has a name and a call count.
	// 3. Each node can have children (subsequent function calls).
	// 4. The root node is the starting point of the call stack.
	// 5. The width of each node in the flame graph is proportional to its call count.
	// 6. The height of each node is fixed (for visual clarity).
	// 7. Nodes are stacked horizontally to represent the call hierarchy.
	// 8. Leaf nodes (functions with no children) are at the bottom of the stack.
	// 9. Non-leaf nodes call count is the sum of its children's call counts.
	// 10.0 Leaf node call count is the number of times the function was called.

	struct FunctionData
	{
		std::string name;
		std::string id;

		int callCount = 0;
		int level = 0;

		std::string callname;
		ImVec4 color = ImVec4(0.0f, 0.0f, 0.0f, 0.5f);

		std::unordered_map<std::string, FunctionData*> children;
		FunctionData* parent = nullptr;

		bool IsLeaf() const
		{
			return children.empty();
		}

		bool IsRoot() const
		{
			return parent == nullptr;
		}

		bool HasColor() const
		{
			return (this->color.x != 0.0f) && (this->color.y != 0.0f) && (this->color.z != 0.0f);
		}

		void AddChild(FunctionData* child)
		{
			// This signature identifies first-level functions on the graph.
			if (this->parent && this->parent->parent == nullptr && this->children.empty()) {
				this->GenerateRandomColor();
			}

			// Otherwise we create duplicates by counting the log.
			if (children.find(child->id) == children.end()) {
				children[child->id] = child;
				child->parent = this;
				child->GenerateLeafColor();
			}

			if (child->ExtractFunctionTag() == this->ExtractFunctionTag()) {
				child->callname = child->ExtractFunctionCall();
			}
		}

		// Function to get total call count, but handle the "all" root case
		int GetCallCount() const
		{
			if (IsRoot()) {
				int totalCallCount = 0;
				for (const auto& [identifier, child] : children) {
					totalCallCount += child->callCount;
				}
				return totalCallCount;
			}

			return callCount;
		}

		int GetCallCountAlt() const
		{
			if (!IsLeaf()) {
				int totalCallCount = 0;
				for (const auto& [identifier, child] : children) {
					totalCallCount += child->callCount;
				}
				return totalCallCount;
			}

			return callCount;
		}

		int GetCumlativeCallCount() const
		{
			int totalCallCount = callCount;

			for (const auto& [identifier, child] : children) {
				totalCallCount += child->GetCumlativeCallCount();
			}

			return totalCallCount;
		}

		void GenerateRandomColor()
		{
			static std::mt19937 rng(std::random_device{}());
			std::uniform_real_distribution<float> dist(0.0f, 1.0f);
			this->color = ImVec4(dist(rng), dist(rng), dist(rng), 1.0f);
		}

		void GenerateLeafColor() noexcept
		{
			const auto col = this->parent->color;
			this->color = ImVec4(col.x * 0.8f, col.y * 0.8f, col.z * 0.8f, 1.0f);
		}

		// Simply returns the function identifier after the period.
		std::string ExtractFunctionTag()
		{
			size_t dotPos = this->name.find('.');

			if (dotPos != std::string::npos) {
				return this->name.substr(0, dotPos);
			}

			return this->name;
		}

		std::string ExtractFunctionCall()
		{
			size_t dotPos = this->name.find_last_of('.');

			if (dotPos != std::string::npos) {
				return this->name.substr(dotPos + 1);
			}

			return this->name;
		}
	};

	class LogReader
	{
	public:
		void ResetData()
		{
			lastPosition = std::streampos(0);
			functionData.clear();

			functionData["root"] = FunctionData{ "All Scripts", "root" };  // Initialize with a root function
		}

		void UpdateLog(const std::filesystem::path& a_filePath)
		{
			std::ifstream file;

			file.open(a_filePath, std::ios::in);

			if (!file.is_open()) {
				logger::error("[LogReader] Error: Could not open file: {}", a_filePath.string());
				return;
			}

			// pickup from last
			if (lastPosition != std::streampos(0)) {
				file.seekg(lastPosition);
			}

			std::string line;
			while (std::getline(file, line)) {
				std::stringstream ss(line);
				std::string part;

				// Each line represents a complete call stack in the log.
				// Use the ";" delimiter to split the line into individual function calls.
				std::string parentName = "root";
				std::string identifier;

				while (std::getline(ss, part, ';')) {
					std::regex functionRegex(R"(([A-Za-z0-9_]+(?:\.[A-Za-z0-9_]+)*))");
					std::smatch match;
					if (std::regex_search(part, match, functionRegex)) {
						std::string functionName = match[0];
						identifier = parentName + std::string(" -> ") + functionName;

						// If function doesn't exist, create it using full stack name
						if (functionData.find(identifier) == functionData.end()) {
							functionData.emplace(identifier, FunctionData{ functionName, identifier });
						}

						// logger::info("[LogReader] Found function: {}", identifier);  // TODO: Fix formatting here, check logs
						functionData[identifier].callCount++;

						// Get the parent function
						FunctionData* parentFunction = &functionData[parentName];
						parentFunction->AddChild(&functionData[identifier]);

						// Update parent to the current function
						parentName = identifier;
					}
				}

				// Reset identifier for the next line
				identifier.clear();
				parentName = "root";
			}

			if (file.eof()) {
				file.clear();  // Clear the EOF flag for tellg
			}

			lastPosition = file.tellg();  // Update the last position to the current file pointer

			file.close();
		}

		uint32_t GetLastPos() const
		{
			return static_cast<uint32_t>(lastPosition);
		}

		const std::unordered_map<std::string, FunctionData>& GetFunctionData() const
		{
			return functionData;
		}

	private:
		std::streampos lastPosition;  // The last read position in the file
		std::unordered_map<std::string, FunctionData> functionData;
		// std::unordered_map<std::string, int> functionCount;
	};
}