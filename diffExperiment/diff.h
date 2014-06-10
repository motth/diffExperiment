/***********************************************************************************************************************
**
** Copyright (c) 2011, 2014 ETH Zurich
** All rights reserved.
**
** Redistribution and use in source and binary forms, with or without modification, are permitted provided that the
** following conditions are met:
**
** * Redistributions of source code must retain the above copyright notice, this list of conditions and the following
** disclaimer.
** * Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the
** following disclaimer in the documentation and/or other materials provided with the distribution.
** * Neither the name of the ETH Zurich nor the names of its contributors may be used to endorse or promote products
** derived from this software without specific prior written permission.
**
**
** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES,
** INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
** DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR
** SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY,
** WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
**
***********************************************************************************************************************/

#ifndef DIFF_H
#define DIFF_H

#include <git2.h>

#include <QHash>

#include <astdiff.h>
#include <astnode.h>
#include <astnodediff.h>

typedef enum {IndexToWorkdir, TreeToWorkdir, TreeToIndex, TreeToTree} diffKind;

class Diff
{
	public:
		Diff(git_repository* repository);
		~Diff();

		void diffIndexToWorkdir();
		void diffTreeToWorkdir(git_tree* oldTree);
		void diffTreeToIndex(git_tree* oldTree);
		void diffTreeToTree(git_tree* oldTree, git_tree* newTree);


	private:
		void checkError(int error);

		void initializeOptions();
		void postProcessDiff();

		QHash<int, ASTNode*>* modifiedNodes_;

		QHash<int, ASTNodeDiff*>* nodeDiffHash_;

		ASTDiff* astDiff_;

		diffKind diffKind_;
		git_diff_options* options_;
		git_diff* diff_;

		git_tree* oldTree_;
		git_tree* newTree_;

		git_repository* repository_;
};

#endif // DIFF_H
