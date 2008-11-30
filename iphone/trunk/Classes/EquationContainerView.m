//
//  EquationContainerView.m
//  mathomatic
//
//  Created by Ben Gotow on 11/14/08.
//  Copyright 2008 __MyCompanyName__. All rights reserved.
//

#import "EquationContainerView.h"
#import "EquationDivisionView.h"
#import "EquationExponentView.h"

@implementation EquationContainerView

@synthesize innerEquations;
@synthesize parent;


- (id)initWithParent:(EquationContainerView*)p
{
    if (self = [super initWithFrame:CGRectZero]) {
        // Initialization code
        
        self.innerEquations = [NSMutableArray array];
        self.backgroundColor = [UIColor clearColor];
        self.clipsToBounds = YES;
        
        innerEquationsMax = -1;
        self.parent = p;
    }
    return self;
}

- (void)setShowOpenParenthesis:(BOOL)showOpen
{
    if (showOpen){
        openParen = [[UILabel alloc] initWithFrame: CGRectZero];
        [openParen setBackgroundColor: [UIColor clearColor]];
        [self addSubview: openParen];
        [openParen setText: @"("];
    }else{
        [openParen removeFromSuperview];
        [openParen release];
        openParen = nil;
    }
}

- (void)setShowCloseParenthesis:(BOOL)showClose
{
    if (showClose){
        closeParen = [[UILabel alloc] initWithFrame: CGRectZero];
        [closeParen setBackgroundColor: [UIColor clearColor]];
        [self addSubview: closeParen];
        [closeParen setText: @")"];
    }else{
        [closeParen removeFromSuperview];
        [closeParen release];
        closeParen = nil;
    }
}

- (EquationAbstractView*)popInnerEquation
{
    EquationAbstractView * view = [[[self.innerEquations lastObject] retain] autorelease];
    [self.innerEquations removeObjectIdenticalTo: view];
    [view removeFromSuperview];
    return view;
}

- (BOOL)addInnerEquation:(EquationAbstractView*)equation
{
    if ((innerEquationsMax == -1) || ([self.innerEquations count] < innerEquationsMax)){
        [self.innerEquations addObject: equation];
        [self addSubview: equation];
    }else{
        NSLog(@"Attempt to attach %@ to full container", [equation description]);
        return NO;
    }
    return YES;
}

- (BOOL)innerEquationIsFull
{
    if ([self.innerEquations count] < innerEquationsMax)
        return NO;
    else
        return YES;
}

- (EquationAbstractView *)innerEquationAtOffset:(int)offset from:(EquationAbstractView*)v
{
    int ii = [innerEquations indexOfObject: v];
    if ((ii+offset < [innerEquations count]) && (ii+offset >= 0))
        return [innerEquations objectAtIndex: ii + offset];
    else
        return nil;
}

- (void)finalizeEquationTree:(BOOL)clean
{
    [self finalizeEquationTreeForClass: [EquationContainerView class] cleanParenthesis:clean];
    [self finalizeEquationTreeForClass: [EquationExponentView class] cleanParenthesis:clean];

    if ([self finalizeEquationTreeForClass: [EquationDivisionView class] cleanParenthesis: clean])
        alignmentMethod = kAlignmentMethodCentered;
    else if ([innerEquations count] == 1)
        alignmentMethod = [[innerEquations lastObject] alignmentMethod];
    else
        alignmentMethod = kAlignmentMethodBottom;
        
    if (([innerEquations count] < 2) && (clean)){
        [self setShowOpenParenthesis: NO];
        [self setShowCloseParenthesis: NO];
    }
}

- (BOOL)finalizeEquationTreeForClass:(Class)c cleanParenthesis:(BOOL)clean
{
    BOOL found = NO;
    int ii = [innerEquations count] - 1;

    while (ii >= 0){
        EquationAbstractView * v = [innerEquations objectAtIndex: ii];
        if ([v isMemberOfClass: c]){
            [v finalizeEquationTree: clean];
            ii = [innerEquations indexOfObject: v];
            
            found = YES;
        }
        ii--;
    }
    return found;
}

- (void)finalizeTextSize:(int)parentTextSize
{
    for (EquationAbstractView * v in self.innerEquations)
        [v finalizeTextSize: parentTextSize];
    
    [openParen setFont: [UIFont fontWithName:@"Courier" size:parentTextSize]];
    [closeParen setFont: [UIFont fontWithName:@"Courier" size:parentTextSize]];
}

- (void)finalizeFrame
{
    [self.innerEquations makeObjectsPerformSelector:@selector(finalizeFrame)];
    
    int heightAboveBaseline = 0;
    int heightBelowBaseline = 0;
    int x = 0;
    
    for (EquationAbstractView * v in self.innerEquations){
        if (v.alignmentMethod == kAlignmentMethodBottom){
            heightAboveBaseline = fmax([v frame].origin.y + [v frame].size.height, heightAboveBaseline);
        } else if (v.alignmentMethod == kAlignmentMethodCentered){
            heightAboveBaseline = fmax(([v frame].origin.y + [v frame].size.height)/2, heightAboveBaseline);
            heightBelowBaseline = fmax(([v frame].origin.y + [v frame].size.height)/2, heightBelowBaseline);
        }
    }
    
    CGSize openSize = [[openParen text] sizeWithFont: [openParen font]];
    CGSize closeSize = [[closeParen text] sizeWithFont: [closeParen font]];
    
    if ([innerEquations count] == 0)
        heightAboveBaseline = openSize.height;
        
    x = openSize.width;
    
    for (EquationAbstractView * v in self.innerEquations){
        int offset;
        if (v.alignmentMethod == kAlignmentMethodBottom)
            offset = (heightBelowBaseline)/2 + heightAboveBaseline-[v frame].size.height;
        else
            offset = heightAboveBaseline - ([v frame].size.height) / 2;
        [v setFrame: CGRectMake(x, offset, [v frame].size.width, [v frame].size.height)];
        x += [v frame].size.width;
    }
    
    int height = fmax(heightAboveBaseline + heightBelowBaseline, openSize.height);

    [openParen setFrame: CGRectMake(0, (heightAboveBaseline + heightBelowBaseline - openSize.height)/2, openSize.width, openSize.height)];
    [closeParen setFrame: CGRectMake(x, (heightAboveBaseline + heightBelowBaseline - closeSize.height)/2, closeSize.width, closeSize.height)];
    [self setFrame: CGRectMake(0,0, x + closeSize.width, height)];
}

- (BOOL)isTreeLegal
{
    for (EquationAbstractView * view in self.innerEquations){
        if (![view isTreeLegal])
            return NO;
    }
    return YES;
}


- (NSString*)description
{
    NSMutableString * description = [NSMutableString stringWithString: @"("];
    for (EquationAbstractView * view in self.innerEquations)
        [description appendString: [view description]];
    [description appendFormat: @")"];
    return description;
}

- (void)dealloc {
    [openParen release];
    [closeParen release];
    [super dealloc];
}


@end
