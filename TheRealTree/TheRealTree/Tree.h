#pragma once
#include "pch.h"

#include <iostream>
#include <memory>
#include <set>
#include <list>
#include <optional>
#include <Windows.h>

const int c_diff = 10;

enum LineState
{
    Straight,
    Diagnol
};

enum XDiffDirection
{
    Left,
    Right,
};

enum YDiffDirection
{
    Up,
    Down
};

struct DotSpan
{
    int xDiff = 0;
    int yDiff = 0;

    DotSpan(int xDiff, int yDiff)
    {
        this->xDiff = xDiff;
        this->yDiff = yDiff;
    }
};

struct Dot
{
    int x{};
    int y{};

    // this is from the new dots prspective.
    // Left means the new dot is to the left of the previous dot
    // Right means the new dot is to the right of the previous dot.
    std::optional<XDiffDirection> xDiffDirectionFromPreviousDot{};

    // Up means the new dot is above the previous dot
    // Down means the new dot is below the previous dot
    std::optional<YDiffDirection> yDiffDirectionFromPreviousDot{};

    std::shared_ptr<Dot> previous{};
    int round{};

    Dot() {}

    Dot(int intialX, int intialY)
    {
        x = intialX;
        y = intialY;
    }

    Dot(int intialX, int intialY, int round)
    {
        x = intialX;
        y = intialY;
        this->round = round;
    }

    Dot(const Dot& right)
    {
        x = right.x;
        y = right.y;

        xDiffDirectionFromPreviousDot = right.xDiffDirectionFromPreviousDot;
        yDiffDirectionFromPreviousDot = right.yDiffDirectionFromPreviousDot;
        previous = right.previous;

        round = right.round;
    }


    Dot Add(DotSpan span)
    {
        Dot newDot(x + span.xDiff, y + span.yDiff, (this->round + 1));

        // I want newDot.previous to be equal to this
        newDot.previous = std::make_shared<Dot>(*this);

        // a positive diff is to the right.
        // This means the new dot is to the right of the previous dot.
        if (span.xDiff > 0)
        {
            newDot.xDiffDirectionFromPreviousDot = XDiffDirection::Right;
        }
        else if (span.xDiff < 0)
        {
            newDot.xDiffDirectionFromPreviousDot = XDiffDirection::Left;
        }

        // a positive diff means the new dot is above the previous dot
        if (span.yDiff > 0)
        {
            newDot.yDiffDirectionFromPreviousDot = YDiffDirection::Down;
        }
        else if (span.yDiff < 0)
        {
            newDot.yDiffDirectionFromPreviousDot = YDiffDirection::Up;
        }

        return newDot;
    }


    bool operator==(const Dot& right) const
    {
        // a dots is equal if the coordinates of the dot and previous equal.
        auto areCordinatesEqual{ (x == right.x) && (y == right.y) };

        if (!areCordinatesEqual)
        {
            return false;
        }

        if (previous.get() == nullptr && right.previous.get() == nullptr)
        {
            return true;
        }

        if (previous.get() != nullptr && right.previous.get() != nullptr)
        {
            if ((previous->x == right.previous->x) && (previous->y == right.previous->y))
            {
                return true;
            }
        }

        // at this point one previous is null and the other is not null
        return false;
    }

    bool AreSamePoint(const Dot& right) const
    {
        return ((x == right.x) && (y == right.y));
    }
};


LineState GetParentRelationship(const Dot& dot)
{
    if (dot.xDiffDirectionFromPreviousDot.has_value() && dot.yDiffDirectionFromPreviousDot.has_value())
    {
        return LineState::Diagnol;
    }
    else
    {
        return LineState::Straight;
    }
}

std::list<Dot> RemoveDotsThatAreOnTheSamePoint(const std::list<Dot>& dotsToProcess, const std::list<Dot>& dots)
{
    // another rule is that if two dots have the same coordinates they should be excluded.
    // N^2 baby.

    // First get all the dots that point to the same point.
    std::list<std::tuple<int, int>> dotsToRemove{};
    for (auto& dotToProcess : dotsToProcess)
    {
        for (auto& dotAgainToProcess : dots)
        {
            // I could write a != overload but i'm lazy.
            // Don't process if the two dots are the same
            if (dotToProcess == dotAgainToProcess)
            {
                continue;
            }
            else
            {
                if (dotToProcess.AreSamePoint(dotAgainToProcess))
                {
                    dotsToRemove.push_back({ dotToProcess.x, dotToProcess.y });
                }
            }
        }
    }

    // Remove.
    std::list<Dot> toReturn{ dotsToProcess };
    for (auto& dotToRemove : dotsToRemove)
    {
        toReturn.remove_if([dotToRemove](Dot dot)
            {
                return ((std::get<0>(dotToRemove) == dot.x) && (std::get<1>(dotToRemove) == dot.y));
            });
    }

    return toReturn;
}

std::list<Dot> RemoveViceVersaDots(const std::list<Dot>& dots)
{
    // I don't really know what these dots are called.
    // Turns out there are some dot pairs where dot[0].previous = dot[1] and dot[1].previous = dot[0]
    // These need to be excluded too.
    std::list<std::tuple<int, int>> dotsToRemove{};

    for (auto dot : dots)
    {
        for (auto dotAgain : dots)
        {
            if (dot.AreSamePoint(*dotAgain.previous.get()))
            {
                dotsToRemove.push_back({ dot.x, dot.y });
            }
        }
    }

    // Remove.
    std::list<Dot> toReturn{ dots };
    for (auto& dotToRemove : dotsToRemove)
    {
        toReturn.remove_if([dotToRemove](Dot dot)
            {
                return ((std::get<0>(dotToRemove) == dot.x) && (std::get<1>(dotToRemove) == dot.y));
            });
    }

    return toReturn;
}

std::list<Dot> RemoveDotsThatWouldCrossTheLine(const std::list<Dot>& dotsToProcess, const std::list<Dot>& dots)
{
    // Some dots exist that, when expanded, the resulting lines would cross other lines.
    // Can't have that.
    // I don't know the "best" way to figure this.
    // Lazy.  See if a dot is surrounded. If it is remove it.
    std::list<std::tuple<int, int>> dotsToRemove{};


    for (auto dotToProcess : dotsToProcess)
    {
        bool hasUpNeighbor{};
        bool hasLeftNeighbor{};
        bool hasDownNeighbor{};
        bool hasRightNeighbor{};
        if (GetParentRelationship(dotToProcess) == LineState::Straight)
        {
            Dot upNeighbor{ dotToProcess.x, dotToProcess.y - c_diff };
            Dot leftNeighbor{ dotToProcess.x - c_diff, dotToProcess.y };
            Dot downNeighbor{ dotToProcess.x, dotToProcess.y + c_diff };
            Dot rightNeighbor{ dotToProcess.x + c_diff, dotToProcess.y };

            for (auto dot : dots)
            {
                if (dot.AreSamePoint(upNeighbor))
                {
                    hasUpNeighbor = true;
                }
                else if (dot.AreSamePoint(downNeighbor))
                {
                    hasDownNeighbor = true;
                }
                else if (dot.AreSamePoint(leftNeighbor))
                {
                    hasLeftNeighbor = true;
                }
                else if (dot.AreSamePoint(rightNeighbor))
                {
                    hasRightNeighbor = true;
                }
            }

            if (hasUpNeighbor && hasDownNeighbor && hasLeftNeighbor && hasRightNeighbor)
            {
                dotsToRemove.push_back({ dotToProcess.x, dotToProcess.y });
            }
        }
        else
        {
            Dot upperLeftNeighbor{ dotToProcess.x - c_diff, dotToProcess.y - c_diff };
            Dot lowerLeftNeighbor{ dotToProcess.x - c_diff, dotToProcess.y + c_diff };
            Dot upperRightNeighbor{ dotToProcess.x + c_diff, dotToProcess.y - c_diff };
            Dot lowerRightNeighbor{ dotToProcess.x + c_diff, dotToProcess.y + c_diff };

            for (auto dot : dots)
            {
                if (dot.AreSamePoint(upperLeftNeighbor))
                {
                    hasUpNeighbor = true;
                }
                else if (dot.AreSamePoint(lowerLeftNeighbor))
                {
                    hasDownNeighbor = true;
                }
                else if (dot.AreSamePoint(upperRightNeighbor))
                {
                    hasLeftNeighbor = true;
                }
                else if (dot.AreSamePoint(lowerRightNeighbor))
                {
                    hasRightNeighbor = true;
                }
            }

            if (hasUpNeighbor && hasDownNeighbor && hasLeftNeighbor && hasRightNeighbor)
            {
                dotsToRemove.push_back({ dotToProcess.x, dotToProcess.y });
            }
        }
    }

    // Remove.
    std::list<Dot> toReturn{ dotsToProcess };
    for (auto& dotToRemove : dotsToRemove)
    {
        toReturn.remove_if([dotToRemove](Dot dot)
            {
                return ((std::get<0>(dotToRemove) == dot.x) && (std::get<1>(dotToRemove) == dot.y));
            });
    }

    return toReturn;
}


static std::list<Dot> MakeDots(int width, int heigth, int numberOfRounds)
{
    std::list<Dot> dots{};

    Dot startingDot(width / 2, heigth / 2, 0);
    dots.push_back(startingDot);

    std::list<Dot> dotsToProcess{};

    // The rule is that a single line comes from the starting dot.
    DotSpan span(0, 0 - c_diff);
    Dot nextDot = startingDot.Add(span);
    dots.push_back(nextDot);
    dotsToProcess.push_back(nextDot);

    LineState lineState{ Diagnol };
    // -2 already made two rounds above.
    for (int currentRound = 0; currentRound < (numberOfRounds - 2); currentRound++)
    {
        dotsToProcess = RemoveDotsThatAreOnTheSamePoint(dotsToProcess, dots);
        dotsToProcess = RemoveViceVersaDots(dotsToProcess);
        dotsToProcess = RemoveDotsThatWouldCrossTheLine(dotsToProcess, dots);
        std::list<Dot> tempDotsToAdd{};
        // Each dot will produce a maximum of 2 dots to process.
        for (auto&& dotToProcess : dotsToProcess)
        {
            std::optional<XDiffDirection> xDiffDirectionFromPreviousDot{ dotToProcess.xDiffDirectionFromPreviousDot };
            std::optional<YDiffDirection> yDiffDirectionFromPreviousDot{ dotToProcess.yDiffDirectionFromPreviousDot };

            // from the diff from this dot from the previous dot, and
            // from the line state, we can
            // figure out the new dots.

            int xDiffToNewDot{};
            int yDiffToNewDot{};

            // if line state is diagnol it means we came from a stright line.
            // if like state is stright, it means we came from a diagnol.
            if (lineState == LineState::Diagnol)
            {
                // If here it means we came from a stright line and
                // both diffs will be used.
                // If we are here it means the next dots will be offset diagonally.
                // directions are either up, down, left, or right.  No mixing since this came from
                // a stright line state.
                if (xDiffDirectionFromPreviousDot.has_value())
                {
                    // Means the new dot is to the left of the previous dot
                    if (xDiffDirectionFromPreviousDot == XDiffDirection::Left)
                    {
                        // split up left
                        xDiffToNewDot -= c_diff;
                        yDiffToNewDot -= c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));

                        // Reset
                        xDiffToNewDot = 0;
                        yDiffToNewDot = 0;

                        // split down left
                        xDiffToNewDot -= c_diff;
                        yDiffToNewDot += c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));
                    }
                    else // new dot is to the right of the previous dot
                    {
                        // split up right
                        xDiffToNewDot += c_diff;
                        yDiffToNewDot -= c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));

                        // Reset
                        xDiffToNewDot = 0;
                        yDiffToNewDot = 0;

                        // split down right
                        xDiffToNewDot += c_diff;
                        yDiffToNewDot += c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));
                    }
                }

                if (yDiffDirectionFromPreviousDot.has_value())
                {
                    // Means the new dot is above the previous dot
                    if (yDiffDirectionFromPreviousDot == YDiffDirection::Up)
                    {
                        // split up left
                        xDiffToNewDot -= c_diff;
                        yDiffToNewDot -= c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));

                        // Reset
                        xDiffToNewDot = 0;
                        yDiffToNewDot = 0;

                        // split up right
                        xDiffToNewDot += c_diff;
                        yDiffToNewDot -= c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));
                    }
                    else
                    {
                        // split down left
                        xDiffToNewDot -= c_diff;
                        yDiffToNewDot += c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));

                        // Reset
                        xDiffToNewDot = 0;
                        yDiffToNewDot = 0;

                        // split down right
                        xDiffToNewDot += c_diff;
                        yDiffToNewDot += c_diff;
                        tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, yDiffToNewDot }));
                    }
                }
            }
            else
            {
                // The dot we came from was diagonally away from us.
                // both xdiff and y diff should have values since a diagnol move requires an x and y change.
                if (xDiffDirectionFromPreviousDot.has_value() && yDiffDirectionFromPreviousDot.has_value())
                {
                    if (xDiffDirectionFromPreviousDot == XDiffDirection::Left)
                    {
                        if (yDiffDirectionFromPreviousDot == YDiffDirection::Up)
                        {
                            // The new dot is to the upper-left of the previous dot
                            // split left
                            xDiffToNewDot -= c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, 0 }));

                            // split up
                            yDiffToNewDot -= c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ 0, yDiffToNewDot }));

                        }
                        else
                        {
                            // split left
                            xDiffToNewDot -= c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, 0 }));

                            // split down
                            yDiffToNewDot += c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ 0, yDiffToNewDot }));
                        }
                    }
                    else
                    {
                        if (yDiffDirectionFromPreviousDot == YDiffDirection::Up)
                        {
                            // split right
                            xDiffToNewDot += c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, 0 }));

                            // split up
                            yDiffToNewDot -= c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ 0, yDiffToNewDot }));
                        }
                        else
                        {
                            // split right
                            xDiffToNewDot += c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ xDiffToNewDot, 0 }));

                            // split down
                            yDiffToNewDot += c_diff;
                            tempDotsToAdd.push_back(dotToProcess.Add({ 0, yDiffToNewDot }));
                        }
                    }
                }
            }
        }

        // All dots from the previous level have been processed.
        dotsToProcess.clear();

        for (Dot& tempDotToAdd : tempDotsToAdd)
        {
            // check for duplicates
            bool doesTempAlreadyExistInDots{};
            for (Dot& existingDot : dots)
            {
                if (existingDot == tempDotToAdd)
                {
                    doesTempAlreadyExistInDots = true;
                    break;
                }
            }

            if (!doesTempAlreadyExistInDots)
            {
                dots.push_back(tempDotToAdd);
                dotsToProcess.push_back(tempDotToAdd);
            }
        }

        // switch line states.
        if (lineState == LineState::Straight)
        {
            lineState = LineState::Diagnol;
        }
        else
        {
            lineState = LineState::Straight;
        }
    }

    return dots;
}